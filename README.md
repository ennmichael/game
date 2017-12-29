## The game

# Config loading
Currently the configuration files should be written as follows:
```
type name=value
```

Where `type` is either `int` or `str`.

`name` may be any string which doesn't contain the `=` character.

The PNG sheet configs currently store the number of frames for each sheet and the frame delay for each sheet.
The frame delay represents the number of real (in-game) frames to wait before the animation of the sheet advances to it's next frame.

