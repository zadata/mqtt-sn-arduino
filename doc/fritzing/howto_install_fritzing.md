How to install Fritzing on Ubuntu 12.04 LTS 64 bit
==================================================

The official downloads are here:
http://fritzing.org/download/

``` bash
http://fritzing.org/download/0.9.0b/linux-64bit/fritzing-0.9.0b.linux.AMD64.tar_1.bz2
tar xvjf fritzing-0.9.0b.linux.AMD64.tar_1.bz2
#rm fritzing-0.9.0b.linux.AMD64.tar_1.bz2
echo "export PATH=\$PATH:$HOME/fritzing-0.9.0b.linux.AMD64/" >> $HOME/.bashrc
```

To run Fritzing:

``` bash
cd fritzing-0.9.0b.linux.AMD64/
./Fritzing 
```
