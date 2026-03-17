
# Stack frames

Organized like this:

|   |
|---|
| Arg n |
| ... |
| Arg 2 |
| Arg 1 |
| Saved return address |
| Saved bp |  <-- bp
| Local variable 1 |
| Local variable 2 |
| ... |
| Local variable n |
| Local data |
| (free space) |  <-- sp

# Calling conventions

- Exiting a scope should automatically:
    - Pop return value from stack top
    - Go to bp and restore old bp
    - Overwrite saved bp with return value, deleting local variables
- The above is mainly to deal with local variables. For function returns, the above must happen, but also arguments must be dealt with, and return value must be used

