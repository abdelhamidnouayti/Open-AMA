# Open-AMA
To run you must execute the file openama.py in the Scripts folderusing the comand:
```bash
/Scripts$ python openama.py
```
## Requirements
* Install c++ gcc g++
* install anaconda or miniconda
* install PyQt5
* python packages:
	- numpy
	- cartopy
	- matplotlib
	- pandas
	- scipy


For Generating traj_auto:
* Hysplit: linux version
* install expect

For downloading the meteorological data for the trajectory calculations
* install wget

To modify the source code and create a new sheared library (ama.so):
* install swig


## Installation 
to install the requirements of Open-AMA, follow the steps below:
* Install c++ gcc g++ ...
```bash
sudo apt install build-essential
```
* Install anaconda 
	- download anaconda
	- Then
	 ```bash
	bash ~/Downloads/Anaconda3-2021.11-Linux-x86_64.sh
	 ```
	- yes
	- yes
	```bash
	source .bashrc
	```

* crearte the ama enveronment 
	```bash
	conda create -n ama python=3.10
	conda activate ama
	conda install numpy
	conda install -c conda-forge cartopy
	conda install matplotlib
	conda install pandas
	```

* Install PyQt5 and qt designer
	```bash 
	sudo apt-get install python3-pyqt5
	sudo apt-get install pyqt5-dev-tools
	sudo apt-get install qttools5-dev-tools
	```

* Install swig
	```bash
	sudo apt-get install swig
	```

* Create the shared library _ama.so
	```bash
	sudo apt-get install python3.10-dev
	swig -python -c++ ama.i
	gcc -fPIC -c ama.cpp ama_wrap.cxx -I/usr/include/python3.10/
	c++ -shared ama.o ama_wrap.o -o _ama.so
	```

* Generate traj_auto:
	```bash
	sudo apt-get install expect
	pip install wget
	```
	
