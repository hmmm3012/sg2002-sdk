# SG2002 SDK

# How to use ? 

## Basic

1. Clone this repo

- Using SSH (recommend) :

```
git clone --recurse-submodules git@github.com:hmmm3012/sg2002-sdk.git
```

2. Build Docker image 

```
cd sg2002-sdk
docker build -t sg2002-sdk .
```

3. Run Docker container at the root sdk folder 

```
cd sg2002-sdk
docker run -it -v "$(pwd):/workspace" sg2002-sdk
```

4. Set up SDK

```
source ./build/envsetup.sh
defconfig sg2002_wevb_riscv64_sd 
```

5. Build SDK

- Build the whole SDK:
```
build_all
```

# Rules

- After clone, create your own branch then start develop.

# Directories

```
build: build scripts & config files
host-tools: cross-compiling toolchain
osdrv: OS's drivers
oss: compressed 3rd-party libraries
ramdisk: simple rootfs
u-boot-2021.10: u-boot bootloader source
install: built system images
cvi_rtsp: RTSP
cvi_mpi: middleware/multimedia process interface
cviruntime: TPU library
linux_5.10: Linux kernel v5.10 source
fsbl: first stage bootloader
buildroot-2021.05: Buildroot 2021.05 source (build toolchain)

```

# Documents
## SDK related references:
- SG200x SDK repo: https://github.dev/sophgo/sophgo-doc/blob/main/SG200X/Software_Developers_Manual/SG200x_Software_Developer's_Manual_en.md
- TPU_MLIR: https://github.com/sophgo/tpu-mlir
- CVI_MPI: https://doc.sophgo.com/cvitek-develop-docs/master/docs_latest_release/CV180x_CV181x/en/01.software/MPI/Media_Processing_Software_Development_Reference/build/MediaProcessingSoftwareDevelopmentReference_en.pdf
- TDL_SDK: https://doc.sophgo.com/cvitek-develop-docs/master/docs_latest_release/CV180x_CV181x/en/01.software/TPU/TDL_SDK_Software_Development_Guide/build/html/index.html
