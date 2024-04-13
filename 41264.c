static long kvm_vcpu_ioctl(struct file *filp,
			   unsigned int ioctl, unsigned long arg)
{
	struct kvm_vcpu *vcpu = filp->private_data;
	void __user *argp = (void __user *)arg;
	int r;
	struct kvm_fpu *fpu = NULL;
	struct kvm_sregs *kvm_sregs = NULL;

	if (vcpu->kvm->mm != current->mm)
		return -EIO;

#if defined(CONFIG_S390) || defined(CONFIG_PPC)
	/*
	 * Special cases: vcpu ioctls that are asynchronous to vcpu execution,
	 * so vcpu_load() would break it.
	 */
	if (ioctl == KVM_S390_INTERRUPT || ioctl == KVM_INTERRUPT)
		return kvm_arch_vcpu_ioctl(filp, ioctl, arg);
#endif


	vcpu_load(vcpu);
	switch (ioctl) {
	case KVM_RUN:
		r = -EINVAL;
		if (arg)
			goto out;
		r = kvm_arch_vcpu_ioctl_run(vcpu, vcpu->run);
		trace_kvm_userspace_exit(vcpu->run->exit_reason, r);
		break;
	case KVM_GET_REGS: {
		struct kvm_regs *kvm_regs;

		r = -ENOMEM;
		kvm_regs = kzalloc(sizeof(struct kvm_regs), GFP_KERNEL);
		if (!kvm_regs)
			goto out;
		r = kvm_arch_vcpu_ioctl_get_regs(vcpu, kvm_regs);
		if (r)
			goto out_free1;
		r = -EFAULT;
		if (copy_to_user(argp, kvm_regs, sizeof(struct kvm_regs)))
			goto out_free1;
		r = 0;
out_free1:
		kfree(kvm_regs);
		break;
	}
	case KVM_SET_REGS: {
		struct kvm_regs *kvm_regs;

		r = -ENOMEM;
		kvm_regs = kzalloc(sizeof(struct kvm_regs), GFP_KERNEL);
		if (!kvm_regs)
			goto out;
		r = -EFAULT;
		if (copy_from_user(kvm_regs, argp, sizeof(struct kvm_regs)))
			goto out_free2;
		r = kvm_arch_vcpu_ioctl_set_regs(vcpu, kvm_regs);
		if (r)
			goto out_free2;
		r = 0;
out_free2:
		kfree(kvm_regs);
		break;
	}
	case KVM_GET_SREGS: {
		kvm_sregs = kzalloc(sizeof(struct kvm_sregs), GFP_KERNEL);
		r = -ENOMEM;
		if (!kvm_sregs)
			goto out;
		r = kvm_arch_vcpu_ioctl_get_sregs(vcpu, kvm_sregs);
		if (r)
			goto out;
		r = -EFAULT;
		if (copy_to_user(argp, kvm_sregs, sizeof(struct kvm_sregs)))
			goto out;
		r = 0;
		break;
	}
	case KVM_SET_SREGS: {
		kvm_sregs = kmalloc(sizeof(struct kvm_sregs), GFP_KERNEL);
		r = -ENOMEM;
		if (!kvm_sregs)
			goto out;
		r = -EFAULT;
		if (copy_from_user(kvm_sregs, argp, sizeof(struct kvm_sregs)))
			goto out;
		r = kvm_arch_vcpu_ioctl_set_sregs(vcpu, kvm_sregs);
		if (r)
			goto out;
		r = 0;
		break;
	}
	case KVM_GET_MP_STATE: {
		struct kvm_mp_state mp_state;

		r = kvm_arch_vcpu_ioctl_get_mpstate(vcpu, &mp_state);
		if (r)
			goto out;
		r = -EFAULT;
		if (copy_to_user(argp, &mp_state, sizeof mp_state))
			goto out;
		r = 0;
		break;
	}
	case KVM_SET_MP_STATE: {
		struct kvm_mp_state mp_state;

		r = -EFAULT;
		if (copy_from_user(&mp_state, argp, sizeof mp_state))
			goto out;
		r = kvm_arch_vcpu_ioctl_set_mpstate(vcpu, &mp_state);
		if (r)
			goto out;
		r = 0;
		break;
	}
	case KVM_TRANSLATE: {
		struct kvm_translation tr;

		r = -EFAULT;
		if (copy_from_user(&tr, argp, sizeof tr))
			goto out;
		r = kvm_arch_vcpu_ioctl_translate(vcpu, &tr);
		if (r)
			goto out;
		r = -EFAULT;
		if (copy_to_user(argp, &tr, sizeof tr))
			goto out;
		r = 0;
		break;
	}
	case KVM_SET_GUEST_DEBUG: {
		struct kvm_guest_debug dbg;

		r = -EFAULT;
		if (copy_from_user(&dbg, argp, sizeof dbg))
			goto out;
		r = kvm_arch_vcpu_ioctl_set_guest_debug(vcpu, &dbg);
		if (r)
			goto out;
		r = 0;
		break;
	}
	case KVM_SET_SIGNAL_MASK: {
		struct kvm_signal_mask __user *sigmask_arg = argp;
		struct kvm_signal_mask kvm_sigmask;
		sigset_t sigset, *p;

		p = NULL;
		if (argp) {
			r = -EFAULT;
			if (copy_from_user(&kvm_sigmask, argp,
					   sizeof kvm_sigmask))
				goto out;
			r = -EINVAL;
			if (kvm_sigmask.len != sizeof sigset)
				goto out;
			r = -EFAULT;
			if (copy_from_user(&sigset, sigmask_arg->sigset,
					   sizeof sigset))
				goto out;
			p = &sigset;
		}
		r = kvm_vcpu_ioctl_set_sigmask(vcpu, p);
		break;
	}
	case KVM_GET_FPU: {
		fpu = kzalloc(sizeof(struct kvm_fpu), GFP_KERNEL);
		r = -ENOMEM;
		if (!fpu)
			goto out;
		r = kvm_arch_vcpu_ioctl_get_fpu(vcpu, fpu);
		if (r)
			goto out;
		r = -EFAULT;
		if (copy_to_user(argp, fpu, sizeof(struct kvm_fpu)))
			goto out;
		r = 0;
		break;
	}
	case KVM_SET_FPU: {
		fpu = kmalloc(sizeof(struct kvm_fpu), GFP_KERNEL);
		r = -ENOMEM;
		if (!fpu)
			goto out;
		r = -EFAULT;
		if (copy_from_user(fpu, argp, sizeof(struct kvm_fpu)))
			goto out;
		r = kvm_arch_vcpu_ioctl_set_fpu(vcpu, fpu);
		if (r)
			goto out;
		r = 0;
		break;
	}
	default:
		r = kvm_arch_vcpu_ioctl(filp, ioctl, arg);
	}
out:
	vcpu_put(vcpu);
	kfree(fpu);
	kfree(kvm_sregs);
	return r;
}
