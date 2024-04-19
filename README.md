# Nwge Minesweeper

The classic minesweeper game implemented on the nwge engine.

## Build for yourself

You must have the [nwge SDK][nwge] and [bip][bip] 3.0+ installed on your system.
Then, to perform a debug build:

```console
$ bip build
game
  main.o
  MineField.o
  MineFieldState.o
  mine
 OK. 3 objects compiled, 0 objects reused
```

Before you can play, make sure to create the `mine.bndl` bundle:

```console
$ nwgebndl create source/data target/mine.bndl
Writing bundle file version 1
Using buffer size 10000
Packing source/data into bundle target/mine.bndl
Adding icons.png
Adding inter.cfn
Adding ofl.txt
```

Then, copy the `mine` executable (`mine.exe` on Windows) to the `target`
directory:

```sh
cp target/debug/mine target
# or, on Windows
copy target\debug\mine.exe target
```

Then, change into the `target` directory and launch the executable:

```sh
cd target
./mine # or .\mine.exe
```

[nwge]: https://qeaml.github.io/nwge-docs/SDK
[bip]: https://github.com/qeaml/bip
