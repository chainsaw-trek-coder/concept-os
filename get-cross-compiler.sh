sudo apt install flex -y
sudo apt install libgmp3-dev -y
sudo apt install libmpc-dev -y
sudo apt install libmpfr-dev -y
sudo apt install texinfo -y
pushd /tmp

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

# Binutils
wget https://ftp.gnu.org/gnu/binutils/binutils-2.34.tar.gz
tar xvf binutils-2.34.tar.gz
mkdir build-binutils
cd build-binutils
../binutils-2.34/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install

# GCC
wget https://ftp.gnu.org/gnu/gcc/gcc-9.4.0/gcc-9.4.0.tar.gz
tar xvf gcc-9.4.0.tar.gz

which -- $TARGET-as || echo $TARGET-as is not in the PATH

mkdir build-gcc
cd build-gcc
../gcc-9.4.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc -j 2
make all-target-libgcc -j 2
make install-gcc -j 2
make install-target-libgcc -j 2

popd