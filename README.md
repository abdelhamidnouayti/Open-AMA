# Open-AMA
To run you must execute the file openama.py in the Scripts folder.
## Requirements
* Install c++ gcc g++
* install anaconda or miniconda
* install PyQt5 and qt designer 
* python packages:
   - numpy
   - cartopy
   - matplotlib
   - pandas

Generate traj_auto:
* Hysplit: linux version
* install expect
* install wget

To create sheared library:
* install swig

### Install c++ gcc g++ ...
sudo apt install build-essential

### install anaconda 
#### Prerequisites
sudo apt-get install libgl1-mesa-glx libegl1-mesa libxrandr2 libxrandr2 libxss1 libxcursor1 libxcomposite1 libasound2 libxi6 libxtst6
#### download anaconda
https://repo.anaconda.com/archive/Anaconda3-2021.11-Linux-x86_64.sh
bash ~/Downloads/Anaconda3-2021.11-Linux-x86_64.sh
yes
yes
source .bashrc
#uninstall anaconda
rm -rf ~/anaconda3

### crearte the ama enveronment 
conda create -n ama python=3.10
conda activate ama
conda install numpy
conda install -c conda-forge cartopy
conda install matplotlib
conda install pandas



### install PyQt5 and qt designer 
sudo apt-get install python3-pyqt5
sudo apt-get install pyqt5-dev-tools
sudo apt-get install qttools5-dev-tools


### install swig
sudo apt-get install swig

### create the shared library _ama.so
sudo apt-get install python3.10-dev
swig -python -c++ ama.i
gcc -fPIC -c ama.cpp ama_wrap.cxx -I/usr/include/python3.9/
c++ -shared ama.o ama_wrap.o -o _ama.so







	
### Generate traj_auto:
sudo apt-get install expect
pip install wget
	
