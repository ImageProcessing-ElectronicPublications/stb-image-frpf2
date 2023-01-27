# stb-image-frpf2

The image is scaled using "Find Replicant Pixel Fast".

## build

### load submodules

submodules:
- [stb](https://github.com/nothings/stb.git) -> [src](src)

```shell
$ git submodule init
$ git submodule update
```

### install dependencies

build dependencies:

- build-essential
- cmake

```shell
$ sudo apt-get install build-essential cmake
```

### compilation
```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
```
## use

The first and second parameters specify the paths to the image and the result {PNG}.

```shell
./stbimfrpf2 ${IMAGE_PATH} ${IMAGE_PATH}.out.png
```

## structure

- `dependencies.c` - API [stb](https://github.com/nothings/stb.git)
- `frp2.h` - Find Replicant Pixel
- `frpf2.h` - Find Replicant Pixel Fast
- `stb/` - [stb](https://github.com/nothings/stb.git)
- `stbimfrp2.c` - CLI program FRP2.
- `stbimfrpf2.c` - CLI program FRPF2.

## Find Replicant Pixel

Find Replicant Pixel is based on image self-similarity.
Synthesis of the "magnified" image is performed by searching for the most similar neighborhood on the reduced image with a back projection on the original image.
Find Replicant Pixel Fast instead of finding the environment (which is very long) looks for the most similar gradients (center, horizontal, vertical and diagonal).
This allows you to involve "mirror" surroundings in the search.
But because of the loss of characterizing information, the synthesis becomes coarser.

![source](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-frpf2-demo/main/images/butterfly.png)

![destination](https://raw.githubusercontent.com/ImageProcessing-ElectronicPublications/stb-image-frpf2-demo/main/images/butterfly.x4frpf2.png)

---

See [demo and compare](https://github.com/ImageProcessing-ElectronicPublications/stb-image-frpf2-demo).

---

## Links

* [imageworsener](https://github.com/ImageProcessing-ElectronicPublications/imageworsener)
* [stb-image-resize](https://github.com/ImageProcessing-ElectronicPublications/stb-image-resize)
* [stb-image-srcnn](https://github.com/ImageProcessing-ElectronicPublications/stb-image-srcnn)
