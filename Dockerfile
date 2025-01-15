FROM ubuntu:22.04

ARG UID=1000
ARG GID=1000

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
    pkg-config \
    build-essential \
    ninja-build \
    automake \
    autoconf \
    libtool \
    wget \
    curl \
    git \
    gcc \
    libssl-dev \
    bc \
    slib \
    squashfs-tools \
    android-sdk-libsparse-utils \
    jq \
    python3-distutils \
    scons \
    parallel \
    tree \
    python3-dev \
    python3-pip \
    device-tree-compiler \
    ssh \
    cpio \
    fakeroot \
    libncurses5 \
    flex \
    bison \
    libncurses5-dev \
    genext2fs \
    rsync \
    unzip \
    dosfstools \
    mtools \
    tcl \
    openssh-client \
    cmake \
    vim \
    expect \
    xxd 

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1
RUN pip install jinja2
RUN groupadd -g $GID myuser && \
    useradd -u $UID -g myuser -ms /bin/bash myuser
USER myuser
WORKDIR /workspace

CMD ["bash"]