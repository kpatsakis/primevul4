local void use_space(struct space *space)
{
    possess(space->use);
    twist(space->use, BY, +1);
}
