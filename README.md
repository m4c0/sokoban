# sokoban

A clone of the classical japanese warehouse game.

The main bundle contains two applications: the game itself and a map editor.

## Playing the game

The game can be played with a keyboard, using arrows, WASD or HJKL to move.

Press SPACE to restart the level.

Your objective is to fill all targets with a box. Whenever you complete a
level, you automatically go to the next level. There are 60 levels in total.

If you press ESCAPE, you get to the options menu. In there, you can
enable/disable audio, switch to fullscreen or select a previously played level.

## Editing maps

The map editor is available in case you want to create your own levels. It will
overwrite the original levels if you do so - make sure you have backups!

Arrows change levels, ENTER switches to editor mode and `N` adds a new level.

In edit mode:
* Arrows moves the cursor.
* `W` draw walls
* `B` adds a box
* `T` adds a target
* `SPACE` adds an walkable block
* `X` clears a space
* `Z` fills an area with walkable blocks
* `P` changes player starting position
* `ENTER` saves the level
* `ESCAPE` switches back to level navigation (unsaved changes will be lost).

## Copyrights

This implementation is copyright of Eduardo Costa.

Original game design by Hiroyuki Imabayashi, 1982.

Level design is copyright of their respective creators. Designing new levels
using this implementation does not grant the right of selling them with this
implementation or levels not owned by the same author.

