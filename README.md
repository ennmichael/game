## The game

We currently don't have a name for the game... yet. The main character may be called Mike (but probably not, nonetheless `Mike` is what I'll call him in my source code :-) )

# Config loading
Currently the configuration files should be written as follows:

```
name=value
```

`name` may be any string which doesn't contain the `=` character.

The PNG sheet configs currently store the number of frames for each sheet and the frame delay for each sheet.
The frame delay represents the number of real (in-game) frames to wait before the animation of the sheet advances to it's next image.

