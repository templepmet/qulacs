FROM nvcr.io/nvidia/nvhpc:22.7-devel-cuda11.7-ubuntu22.04

ENV DEBIAN_FRONTEND=noninteractive

# Create non-root user
RUN apt-get update && apt-get install -y sudo
ARG USERNAME
ARG UID
ARG GID
RUN groupadd --gid $GID $USERNAME \
	&& useradd --uid $UID --gid $GID -m $USERNAME -s /bin/bash \
	&& echo "$USERNAME ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/$USERNAME \
	&& chmod 0440 /etc/sudoers.d/$USERNAME

RUN ln -sf /usr/share/zoneinfo/Asia/Tokyo /etc/localtime
RUN apt-get update \
	&& apt-get install -y --no-install-recommends \
	ca-certificates \
	less \
	libbz2-dev \
	libncurses-dev \
	libffi-dev \
	libreadline-dev \
	libssl-dev \
	libgsl-dev \
	libltdl-dev \
	libboost-dev
ENV CC=gcc
ENV CXX=g++
ENV LD_LIBRARY_PATH=/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
RUN cd /opt/nvidia/hpc_sdk/Linux_x86_64/22.7/compilers/lib/ && mv libgomp.so.1 libgomp.so.1.bk

# Install cereal
RUN git clone https://github.com/USCiLab/cereal.git -b v1.3.0 --depth 1 /tmp/cereal \
    && cp -r /tmp/cereal/include/cereal /usr/local/include \
    && rm -rf /tmp/cereal

# Install Eigen
RUN git clone https://gitlab.com/libeigen/eigen.git -b 3.3.7 --depth 1 /tmp/eigen \
    && cp -r /tmp/eigen/Eigen /usr/local/include \
    && rm -rf /tmp/eigen

# Install Google Test
RUN git clone https://github.com/google/googletest.git -b release-1.8.1 --depth 1 /tmp/gtest \
    && cp -r /tmp/gtest/googletest/include/gtest /usr/local/include \
    && rm -rf /tmp/gtest

# Install pybind11
RUN git clone https://github.com/pybind/pybind11.git -b v2.10.0 --depth 1 /tmp/pybind11 \
    && cp -r /tmp/pybind11/include/pybind11 /usr/local/include \
    && rm -rf /tmp/pybind11

# Install lcov
RUN git clone https://github.com/linux-test-project/lcov.git -b v1.15 --depth 1 /tmp/lcov \
    && cd /tmp/lcov \
    && make install \
    && cd - \
    && rm -rf /tmp/lcov

# Change non-root user
RUN chown -R $USERNAME /opt
USER ${USERNAME}

# Install pyenv
ENV PYTHON_VERSION=3.9.12
ENV PYENV_ROOT=/opt/pyenv
ENV PATH=${PYENV_ROOT}/shims:${PYENV_ROOT}/bin:${PATH}
RUN git clone https://github.com/pyenv/pyenv.git ${PYENV_ROOT} \
	&& pyenv install -v ${PYTHON_VERSION} \
	&& pyenv global ${PYTHON_VERSION} \
	&& pyenv rehash
RUN pip install -U pip \
    && pip install black flake8 mypy openfermion pybind11-stubgen setuptools

# Install poetry
ENV POETRY_HOME="/opt/poetry"
RUN mkdir ${POETRY_HOME}
RUN curl -sSL https://install.python-poetry.org | POETRY_HOME=${POETRY_HOME} python -

# Settings qulacs
ENV PWD="/workspace/qulacs"
ENV QULACS_PACKAGE="qulacs"
ENV USE_TEST="Yes"
ENV USE_GPU="Yes"
ENV PYTHONPATH="${PWD}/dist:${PYTHONPATH}"
ENV PYTHONPATH="${PWD}/build:${PYTHONPATH}"
