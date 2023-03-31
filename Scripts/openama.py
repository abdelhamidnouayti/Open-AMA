import sys 				# system : OutputGrabber, PyQt5
import time				# OutputGrabber
import threading 		# OutputGrabber
from threading import Thread
import os 				# OutputGrabber
import random
# ================= Graphical interface==================
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5 import uic	# upload file.ui
# =======================================================
import _ama			# C++ shared library
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.colors as mplc
from mpl_toolkits.axes_grid1 import make_axes_locatable
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
import cartopy.crs as ccrs
import cartopy.feature as cfeature
import pandas as pd
import numpy as np
# time lib
import datetime
from dateutil.relativedelta import relativedelta

import wget # download data


class OutputGrabber(QtCore.QThread):
    escape_char = "\b"
    signal_header = QtCore.pyqtSignal(str)
    mysignal = QtCore.pyqtSignal(str)
    def __init__(self, fonction=None, para1=None, para2=None, para3=None, para4=None, para5=None, para6=None,
                 para7=None, para8=None, parent=None, stream=None, threaded=False):
        QtCore.QThread.__init__(self, parent=parent)
        self.fonction = fonction
        self.para1 = para1
        self.para2 = para2
        self.para3 = para3
        self.para4 = para4
        self.para5 = para5
        self.para6 = para6
        self.para7 = para7
        self.para8 = para8
        self.origstream = stream
        self.threaded = threaded
        if self.origstream is None:
            self.origstream = sys.stdout
        self.origstreamfd = self.origstream.fileno()
        # Create a pipe so the stream can be captured:
        self.pipe_out, self.pipe_in = os.pipe()
        self.isRunning = True

    def run(self):
        self.starting()
        if self.fonction == "read_traj":
            try:
                _ama.read_trajectories(self.para1)   # Call your code here
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "clear_traj":
            try:
                _ama.clear_trajs()
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "read_data":
            try:
                if os.path.isfile("../Working/headers.txt"):
                    os.remove("../Working/headers.txt")
                _ama.clear_data()
                self.signal_header.emit("clear")
                _ama.read_data(self.para1, "yes")
                self.signal_header.emit("read")
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "clear_data":
            try:
                if os.path.isfile("../Working/headers.txt"):
                    os.remove("../Working/headers.txt")
                _ama.clear_data()
                self.signal_header.emit("clear")
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "date_format":
            try:
                _ama.date_format(self.para1, self.para2)
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "ctr_trajs":
            try:
                _ama.ctr_trajs(float(self.para1))
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "select_traj":
            try:
                _ama.select_trajs_ctr(float(self.para1), float(self.para2), self.para3)
            except Exception as e:
                self.mysignal.emit(str(e))
        # ======================= CWT =========================
        elif self.fonction == "2D_CWT":
            try:
                _ama.CWT(float(self.para1), float(self.para2), float(self.para3), float(self.para4), float(self.para5))
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "2D_WCWT":
            try:
                _ama.WCWT(self.para1, self.para2, float(self.para3), float(self.para4), float(self.para5), self.para6)
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "3D_CWT":
            try:
                _ama.CWT_3D(float(self.para1), float(self.para2), float(self.para3), float(self.para4), float(self.para5),
                         self.para6)
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "3D_WCWT":
            try:
                _ama.WCWT_3D(self.para1, self.para2, float(self.para3), float(self.para4), float(self.para5), self.para6)
            except Exception as e:
                self.mysignal.emit(str(e))
        # ===================== PSCF =========================
        elif self.fonction == "2D_PSCF":
            try:
                _ama.PSCF(float(self.para1), float(self.para2), float(self.para3), float(self.para4), float(self.para5),
                       float(self.para6))
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "2D_WPSCF":
            try:
                _ama.WPSCF(self.para1, self.para2, float(self.para3), float(self.para4), float(self.para5), self.para6)
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "3D_PSCF":
            try:
                _ama.PSCF_3D(float(self.para1), float(self.para2), float(self.para3), float(self.para4), float(self.para5),
                          self.para6, float(self.para7))
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "3D_WPSCF":
            try:
                _ama.WPSCF_3D(self.para1, self.para2, float(self.para3), float(self.para4), float(self.para5), self.para6)
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "clst_kmean":
            try:
                _ama.clustering_kmean(int(self.para1), int(self.para2), int(self.para3))
            except Exception as e:
                self.mysignal.emit(str(e))
        elif self.fonction == "correlation":
            try:
                _ama.correlation(float(self.para1), float(self.para2), float(self.para3), float(self.para4), float(self.para5), str(self.para6))
            except Exception as e:
                self.mysignal.emit(str(e))
        self.stop()


    def starting(self):
        """
        Start capturing the stream data.
        """
        # Save a copy of the stream:
        self.streamfd = os.dup(self.origstreamfd)
        # Replace the original stream with our write pipe:
        os.dup2(self.pipe_in, self.origstreamfd)
        if self.threaded:
            # Start thread that will read the stream:
            self.workerThread = threading.Thread(target=self.readOutput)
            self.workerThread.start()
            # Make sure that the thread is running and os.read() has executed:
            time.sleep(0.01)

    def stop(self):
        # Print the escape character to make the readOutput method stop:
        self.origstream.write(self.escape_char)
        # Flush the stream to make sure all our data goes in before
        # the escape character:
        self.origstream.flush()
        if self.threaded:
            # wait until the thread finishes so we are sure that
            # we have until the last character:
            self.workerThread.join()
        else:
            self.readOutput()
        # Close the pipe:
        os.close(self.pipe_in)
        os.close(self.pipe_out)
        # Restore the original stream:
        os.dup2(self.streamfd, self.origstreamfd)
        # Close the duplicate stream:
        os.close(self.streamfd)

    def readOutput(self):
        while True:
            char = os.read(self.pipe_out, 1).decode(self.origstream.encoding)
            if not char or self.escape_char in char:
                break
            self.mysignal.emit(str(char))



class Create_subWindow(QtWidgets.QWidget):
    signal_reduce_cwt = QtCore.pyqtSignal(str)
    signal_weight_cwt = QtCore.pyqtSignal(str)
    signal_reduce_pscf = QtCore.pyqtSignal(str)
    signal_weight_pscf = QtCore.pyqtSignal(str)
    signal_roseplot_groups = QtCore.pyqtSignal(str)
    signal_plot_roseplot = QtCore.pyqtSignal(str)


    def __init__(self, number_layers = None, function = None, path_dir=None):
        super(Create_subWindow, self).__init__()
        self.layout = QtWidgets.QGridLayout()
        self.setLayout(self.layout)

        # * * * * * * * * CWT
        self.alt_cwt_function = "No"
        self.number_layers_cwt = 1         # from the spinBox : number of layers
        self.layers_altitude_cwt = ''      # number height of each cwt layer, for example : "500,1000;3000"
        self.nbr_traj_cwt = ''             # number of trajectories in a cell, for example: "30,10,5:50;20;6"
        self.red_ratio_traj_cwt = ''       # # reduce values : get_weight_traj_cwt : "0.7,0.5,0.1:0.8;0.4;0.05"
        self.nbr_endp_cwt = ''             # number of endpoints in a cell, for example: "30,10,5:50;20;6"
        self.red_ratio_endp_cwt = ''       # reduce values: get_weight_endp_cwt : "0.7,0.5,0.1:0.8;0.4;0.05"

        # * * * * * * * * PSCF
        self.alt_pscf_function = "No"
        self.number_layers_pscf = 1         # from the spinBox : number of layers
        self.layers_altitude_pscf = ''      # number height of each pscf layer, for example : "500,1000;3000"
        self.nbr_traj_pscf = ''             # number of trajectories in a cell, for example: "30,10,5:50;20;6"
        self.red_ratio_traj_pscf = ''       # # reduce values : get_weight_traj_cwt : "0.7,0.5,0.1:0.8;0.4;0.05"
        self.nbr_endp_pscf = ''             # number of endpoints in a cell, for example: "30,10,5:50;20;6"
        self.red_ratio_endp_pscf = ''       # reduce values: get_weight_endp_pscf : "0.7,0.5,0.1:0.8;0.4;0.05"

        # * * * * * * * * clustering
        #self.apply_plot_clustering = QtWidgets.QPushButton("apply")
        self.path_dir = ""
        self.function = ""
        self.figure_size = []
        self.clusters_names = []
        self.number_clusters = 0
        self.clust_names = ""
        self.groups_str=""

    def roseplot(self, function, path_dir):
        self.setWindowTitle("RosePlot")
        self.function = function
        self.path_dir = path_dir
        self.lab_clusters_names = QtWidgets.QLabel("Clusters names :")
        self.ledit_clusters_names = QtWidgets.QLineEdit()
        self.lab_discrip_groups = QtWidgets.QLabel("Grouping of trajectories according to their concentrations :")
        self.lab_percentiles = QtWidgets.QLabel("Percentile values :")
        self.ledit_percentiles = QtWidgets.QLineEdit()
        self.btn_groups = QtWidgets.QPushButton("Create groups")
        self.CBox_roseplot= QtWidgets.QComboBox()
        self.btn_addGroupsToData=QtWidgets.QPushButton("Add Groups to Data")
        self.btn_removeGroupsFromData=QtWidgets.QPushButton("Remove Groups from Data")
        self.btn_rosePlot = QtWidgets.QPushButton("Plot")
        self.clearLayout(self.layout)
        self.layout.addWidget(self.lab_clusters_names,1,0,1,1)
        self.layout.addWidget(self.ledit_clusters_names,1,1,1,2)
        #self.layout.addWidget(self.btn_AddClustersToData,1,3,1,1)
        self.layout.addWidget(self.lab_discrip_groups,2,0,1,4)
        self.layout.addWidget(self.lab_percentiles,3,0,1,1)
        self.layout.addWidget(self.ledit_percentiles,3,1,1,2)
        self.layout.addWidget(self.btn_groups,3,3,1,1)
        self.layout.addWidget(self.CBox_roseplot,4,1,1,2)
        self.layout.addWidget(self.btn_addGroupsToData,4,3,1,1)
        self.layout.addWidget(self.btn_removeGroupsFromData,5,3,1,1)
        self.layout.addWidget(self.btn_rosePlot,6,1,1,2)
        for i in range(self.number_clusters):
            if i == 0:
                self.clust_names = self.clusters_names[i]
                continue
            self.clust_names = self.clust_names + f';{self.clusters_names[i]}'
        self.ledit_clusters_names.setText(self.clust_names)
        self.ledit_clusters_names.setReadOnly(True)
        self.ledit_clusters_names.setStyleSheet('background-color:#A9B4B2')
        
        self.btn_groups.clicked.connect(lambda x: self.groups_roseplot())
        """self.btn_groups.clicked.connect(lambda x:self.fill_groups_names())
        self.btn_groups.clicked.connect(lambda x: self.groups_names(self.groups_str))"""
        self.btn_addGroupsToData.clicked.connect(lambda x: _ama.add_groups_to_data_file(self.function, self.path_dir))
        self.btn_removeGroupsFromData.clicked.connect(lambda x: _ama.remove_columns_data("Groups"))
        self.btn_rosePlot.clicked.connect(lambda x: self.emit_signal_to_plot_roseplot())
        self.show()
    
    def groups_roseplot(self):
        self.fill_groups_names()
        self.groups_names(self.groups_str)
    
    def groups_names(self,strin):
        x = strin.split(";")
        for i in range(len(x)):
            self.CBox_roseplot.addItem(x[i])
            
    def fill_groups_names(self):
        try:
            self.groups_str = _ama.fill_group_names(self.ledit_percentiles.text())
        except Exception as e:
            self.mysignal.emit(str(e))

    def emit_signal_to_plot_roseplot(self):
        self.signal_roseplot_groups.emit(str(self.groups_str))
        self.signal_plot_roseplot.emit(str(self.function))
        self.close()
        
    
    def layers_alt_cwt(self, number_layers):
        self.setWindowTitle("Create CWT Layers")
        self.btn_apply_cwt = QtWidgets.QPushButton("apply")
        self.clearLayout(self.layout)
        self.number_layers_cwt = number_layers
        self.altitudes_cwt = [None] * (self.number_layers_cwt - 1)
        self.altitudes_cwt_value = [None] * (self.number_layers_cwt - 1)
        self.discrip = QtWidgets.QLabel("Please, enter the altitudes that limit the CWT Layers")
        self.layout.addWidget(self.discrip, 0, 0, 1, 2)
        for i in range(self.number_layers_cwt - 1):
            self.altitudes_cwt[i] = QtWidgets.QLineEdit()
            self.layer_label = QtWidgets.QLabel(f'Layer # {i + 1} ')
            self.layout.addWidget(self.layer_label, i + 1, 0, 1, 1)
            self.layout.addWidget(self.altitudes_cwt[i], i + 1, 1, 1, 1)
        self.layout.addWidget(self.btn_apply_cwt, self.number_layers_cwt + 1, 1)
        self.btn_apply_cwt.clicked.connect(lambda x: self.get_layers_cwt())
        print("layers_alt_cwt is started")
        self.show()

    def layers_alt_pscf(self, number_layers):
        self.setWindowTitle("Create PSCF layers")
        self.btn_apply_pscf = QtWidgets.QPushButton("apply")
        self.clearLayout(self.layout)
        self.number_layers_pscf = number_layers
        self.altitudes_pscf = [None] * (self.number_layers_pscf - 1)
        self.altitudes_pscf_value = [None] * (self.number_layers_pscf - 1)
        self.discrip = QtWidgets.QLabel("Please, enter the altitudes that limit the PSCF Layers")
        self.layout.addWidget(self.discrip, 0, 0, 1, 2)
        for i in range(self.number_layers_pscf - 1):
            self.altitudes_pscf[i] = QtWidgets.QLineEdit()
            self.layer_label = QtWidgets.QLabel(f'Layer # {i + 1} ')
            self.layout.addWidget(self.layer_label, i + 1, 0, 1, 1)
            self.layout.addWidget(self.altitudes_pscf[i], i + 1, 1, 1, 1)
        self.layout.addWidget(self.btn_apply_pscf, self.number_layers_pscf + 1, 1)
        self.btn_apply_pscf.clicked.connect(lambda x: self.get_layers_pscf())
        print("layers_alt_pscf is started")
        self.show()
        
    def layers_weight_traj_cwt(self):
        self.setWindowTitle("Weight trajectories: CWT")
        self.btn_apply_weight_traj_cwt = QtWidgets.QPushButton("apply")
        self.clearLayout(self.layout)
        self.weight_trajectories_cwt = [None] * self.number_layers_cwt
        self.reduce_ratio_cwt = [None] * self.number_layers_cwt
        self.discrip = QtWidgets.QLabel(
            "Please, enter the number of trajectories in each cell and the reduce values for each layer :")
        self.hight_column = QtWidgets.QLabel("")
        self.traj_column = QtWidgets.QLabel("Number of Trajectories\nin each cell")
        self.reduce_column = QtWidgets.QLabel("Reduce Ratio ")
        self.layout.addWidget(self.discrip, 0, 0, 1, 3)
        self.layout.addWidget(self.hight_column, 1, 0, 1, 1)
        self.layout.addWidget(self.traj_column, 1, 1, 1, 1)
        self.layout.addWidget(self.reduce_column, 1, 2, 1, 1)
        for i in range(self.number_layers_cwt):
            print(i)
            self.weight_trajectories_cwt[i] = QtWidgets.QLineEdit()
            self.reduce_ratio_cwt[i] = QtWidgets.QLineEdit()
            if i == 0:
                self.hight_label = QtWidgets.QLabel(f" 0 m < Altitude < {self.altitudes_cwt_value[i]} m")
                self.layout.addWidget(self.hight_label, i + 2, 0, 1, 1)
                self.layout.addWidget(self.weight_trajectories_cwt[i], i+2, 1, 1, 1)
                self.layout.addWidget(self.reduce_ratio_cwt[i], i+2, 2, 1, 1)
            elif i == self.number_layers_cwt-1:
                self.hight_label = QtWidgets.QLabel(f" {self.altitudes_cwt_value[i-1]} m < Altitude ")
                self.layout.addWidget(self.hight_label, i + 2, 0, 1, 1)
                self.layout.addWidget(self.weight_trajectories_cwt[i], i + 2, 1, 1, 1)
                self.layout.addWidget (self.reduce_ratio_cwt[i], i + 2, 2, 1, 1)
            else:
                self.hight_label = QtWidgets.QLabel(f" {self.altitudes_cwt_value[i-1]} m < Altitude < {self.altitudes_cwt_value[i]} m")
                self.layout.addWidget(self.hight_label, i+2, 0, 1, 1)
                self.layout.addWidget(self.weight_trajectories_cwt[i], i + 2, 1, 1, 1)
                self.layout.addWidget(self.reduce_ratio_cwt[i], i + 2, 2, 1, 1)
        self.layout.addWidget(self.btn_apply_weight_traj_cwt)
        self.btn_apply_weight_traj_cwt.clicked.connect(lambda x: self.get_weight_traj_cwt())
        print("layers_weight_traj_cwt is started")
        self.show()
        
    def layers_weight_endp_cwt(self):
        self.setWindowTitle("Weight endpoints: CWT")
        self.btn_apply_weight_endp_cwt = QtWidgets.QPushButton("Apply")
        self.clearLayout(self.layout)
        self.weight_endpoints_cwt = [None] * self.number_layers_cwt
        self.reduce_ratio_cwt = [None] * self.number_layers_cwt
        self.discrip = QtWidgets.QLabel(
            "Please, enter the number of endpoints in each cell and the reduce values for each layer :")
        self.hight_column = QtWidgets.QLabel("")
        self.endp_column = QtWidgets.QLabel("Number of Endpoints\nin each cell")
        self.reduce_column = QtWidgets.QLabel("Reduce Ratio ")
        self.layout.addWidget(self.discrip, 0, 0, 1, 3)
        self.layout.addWidget(self.hight_column, 1, 0, 1, 1)
        self.layout.addWidget(self.endp_column, 1, 1, 1, 1)
        self.layout.addWidget(self.reduce_column, 1, 2, 1, 1)
        for i in range(self.number_layers_cwt):
            print(i)
            print("layers_weight_endp_cwt is running")
            self.weight_endpoints_cwt[i] = QtWidgets.QLineEdit()
            self.reduce_ratio_cwt[i] = QtWidgets.QLineEdit()
            if i == 0:
                self.hight_label = QtWidgets.QLabel(f" 0 m < Altitude < {self.altitudes_cwt_value[i]} m")
                self.layout.addWidget(self.hight_label, i + 2, 0, 1, 1)
                self.layout.addWidget(self.weight_endpoints_cwt[i], i+2, 1, 1, 1)
                self.layout.addWidget(self.reduce_ratio_cwt[i], i+2, 2, 1, 1)
            elif i == self.number_layers_cwt - 1:
                self.hight_label = QtWidgets.QLabel(f" {self.altitudes_cwt_value[i-1]} m < Altitude ")
                self.layout.addWidget(self.hight_label, i + 2, 0, 1, 1)
                self.layout.addWidget(self.weight_endpoints_cwt[i], i + 2, 1, 1, 1)
                self.layout.addWidget (self.reduce_ratio_cwt[i], i + 2, 2, 1, 1)
            else:
                self.hight_label = QtWidgets.QLabel(f" {self.altitudes_cwt_value[i-1]} m < Altitude < {self.altitudes_cwt_value[i]} m ")
                self.layout.addWidget(self.hight_label, i+2, 0, 1, 1)
                self.layout.addWidget(self.weight_endpoints_cwt[i], i + 2, 1, 1, 1)
                self.layout.addWidget(self.reduce_ratio_cwt[i], i + 2, 2, 1, 1)
        self.layout.addWidget(self.btn_apply_weight_endp_cwt)
        self.btn_apply_weight_endp_cwt.clicked.connect(lambda x: self.get_weight_endp_cwt())
        print("layers_weight_endp_cwt is started")
        self.show()

    def layers_weight_traj_pscf(self):
        self.setWindowTitle("Weight trajectories: PSCF")
        self.btn_apply_weight_traj_pscf = QtWidgets.QPushButton("apply")
        self.clearLayout(self.layout)
        self.weight_trajectories_pscf = [None] * self.number_layers_pscf
        self.reduce_ratio_pscf = [None] * self.number_layers_pscf
        self.discrip = QtWidgets.QLabel(
            "Please, enter the number of trajectories in each cell and the reduce values for each layer :")
        self.hight_column = QtWidgets.QLabel("")
        self.traj_column = QtWidgets.QLabel("Number of Trajectories\nin each cell")
        self.reduce_column = QtWidgets.QLabel("Reduce Ratio ")
        self.layout.addWidget(self.discrip, 0, 0, 1, 3)
        self.layout.addWidget(self.hight_column, 1, 0, 1, 1)
        self.layout.addWidget(self.traj_column, 1, 1, 1, 1)
        self.layout.addWidget(self.reduce_column, 1, 2, 1, 1)
        for i in range(self.number_layers_pscf):
            print(i)
            self.weight_trajectories_pscf[i] = QtWidgets.QLineEdit()
            self.reduce_ratio_pscf[i] = QtWidgets.QLineEdit()
            if i == 0:
                self.hight_label = QtWidgets.QLabel(f" 0 m < Altitude < {self.altitudes_pscf_value[i]} m")
                self.layout.addWidget(self.hight_label, i + 2, 0, 1, 1)
                self.layout.addWidget(self.weight_trajectories_pscf[i], i+2, 1, 1, 1)
                self.layout.addWidget(self.reduce_ratio_pscf[i], i+2, 2, 1, 1)
            elif i == self.number_layers_pscf-1:
                self.hight_label = QtWidgets.QLabel(f" {self.altitudes_pscf_value[i-1]} m < Altitude ")
                self.layout.addWidget(self.hight_label, i + 2, 0, 1, 1)
                self.layout.addWidget(self.weight_trajectories_pscf[i], i + 2, 1, 1, 1)
                self.layout.addWidget (self.reduce_ratio_pscf[i], i + 2, 2, 1, 1)
            else:
                self.hight_label = QtWidgets.QLabel(f" {self.altitudes_pscf_value[i-1]} m < Altitude < {self.altitudes_pscf_value[i]} m")
                self.layout.addWidget(self.hight_label, i+2, 0, 1, 1)
                self.layout.addWidget(self.weight_trajectories_pscf[i], i + 2, 1, 1, 1)
                self.layout.addWidget(self.reduce_ratio_pscf[i], i + 2, 2, 1, 1)
        self.layout.addWidget(self.btn_apply_weight_traj_pscf)
        self.btn_apply_weight_traj_pscf.clicked.connect(lambda x: self.get_weight_traj_pscf())
        print("layers_weight_traj_pscf is started")
        self.show()

    def layers_weight_endp_pscf(self):
        self.setWindowTitle("Weight endpoints: PSCF")
        self.btn_apply_weight_endp_pscf = QtWidgets.QPushButton("Apply")
        self.clearLayout(self.layout)
        self.weight_endpoints_pscf = [None] * self.number_layers_pscf
        self.reduce_ratio_pscf = [None] * self.number_layers_pscf
        self.discrip = QtWidgets.QLabel(
            "Please, enter the number of endpoints in each cell and the reduce values for each layer :")
        self.hight_column = QtWidgets.QLabel("")
        self.endp_column = QtWidgets.QLabel("Number of Endpoints\nin each cell")
        self.reduce_column = QtWidgets.QLabel("Reduce Ratio ")
        self.layout.addWidget(self.discrip, 0, 0, 1, 3)
        self.layout.addWidget(self.hight_column, 1, 0, 1, 1)
        self.layout.addWidget(self.endp_column, 1, 1, 1, 1)
        self.layout.addWidget(self.reduce_column, 1, 2, 1, 1)
        for i in range(self.number_layers_pscf):
            print(i)
            print("layers_weight_endp_pscf is running")
            self.weight_endpoints_pscf[i] = QtWidgets.QLineEdit()
            self.reduce_ratio_pscf[i] = QtWidgets.QLineEdit()
            if i == 0:
                self.hight_label = QtWidgets.QLabel(f" 0 m < Altitude < {self.altitudes_pscf_value[i]} m")
                self.layout.addWidget(self.hight_label, i + 2, 0, 1, 1)
                self.layout.addWidget(self.weight_endpoints_pscf[i], i+2, 1, 1, 1)
                self.layout.addWidget(self.reduce_ratio_pscf[i], i+2, 2, 1, 1)
            elif i == self.number_layers_pscf - 1:
                self.hight_label = QtWidgets.QLabel(f" {self.altitudes_pscf_value[i-1]} m < Altitude ")
                self.layout.addWidget(self.hight_label, i + 2, 0, 1, 1)
                self.layout.addWidget(self.weight_endpoints_pscf[i], i + 2, 1, 1, 1)
                self.layout.addWidget (self.reduce_ratio_pscf[i], i + 2, 2, 1, 1)
            else:
                self.hight_label = QtWidgets.QLabel(f" {self.altitudes_pscf_value[i-1]} m < Altitude < {self.altitudes_pscf_value[i]} m ")
                self.layout.addWidget(self.hight_label, i+2, 0, 1, 1)
                self.layout.addWidget(self.weight_endpoints_pscf[i], i + 2, 1, 1, 1)
                self.layout.addWidget(self.reduce_ratio_pscf[i], i + 2, 2, 1, 1)
        self.layout.addWidget(self.btn_apply_weight_endp_pscf)
        self.btn_apply_weight_endp_pscf.clicked.connect(lambda x: self.get_weight_endp_pscf())
        print("layers_weight_endp_pscf is started")
        self.show()

    def figure_plot_clustering(self, number_clusters):
        self.setWindowTitle("Clusters Figure")
        self.btn_apply_plot_clustering = QtWidgets.QPushButton("apply")
        self.clearLayout(self.layout)
        self.discrip = QtWidgets.QLabel("Please, enter the figure size")
        self.top_lab = QtWidgets.QLabel("Top")
        self.bottom_lab = QtWidgets.QLabel("Bottom")
        self.right_lab = QtWidgets.QLabel("Right")
        self.left_lab = QtWidgets.QLabel("Left")
        self.top_ledit= QtWidgets.QLineEdit()
        self.bottom_ledit = QtWidgets.QLineEdit()
        self.right_ledit = QtWidgets.QLineEdit()
        self.left_ledit = QtWidgets.QLineEdit()
        self.layout.addWidget(self.discrip, 0, 0, 1, 4)
        self.layout.addWidget(self.top_lab, 1, 0, 1, 1)
        self.layout.addWidget(self.bottom_lab, 1, 1, 1, 1)
        self.layout.addWidget(self.left_lab, 1, 2, 1, 1)
        self.layout.addWidget(self.right_lab, 1, 3, 1, 1)
        self.layout.addWidget(self.top_ledit, 2, 0, 1, 1)
        self.layout.addWidget(self.bottom_ledit, 2, 1, 1, 1)
        self.layout.addWidget(self.left_ledit, 2, 2, 1, 1)
        self.layout.addWidget(self.right_ledit, 2, 3, 1, 1)
        self.number_clusters = number_clusters
        self.ledit_cluster_name = [None] * (self.number_clusters)
        self.discrip1 = QtWidgets.QLabel("Please, enter the names of clusters")
        self.layout.addWidget(self.discrip1, 3, 0, 1, 4)
        for i in range(self.number_clusters):
            self.ledit_cluster_name[i] = QtWidgets.QLineEdit()
            self.layer_label = QtWidgets.QLabel(f'Cluster {i + 1}')
            self.layout.addWidget(self.layer_label, i + 4, 1, 1, 1)
            self.layout.addWidget(self.ledit_cluster_name[i], i + 4, 2, 1, 2)
        self.layout.addWidget(self.btn_apply_plot_clustering, self.number_clusters+4, 1, 1, 2)
        self.btn_apply_plot_clustering.clicked.connect(lambda x: self.get_clusters_size_names())
        self.show()

    def get_clusters_size_names(self):
        print("get_clusters_names is started")
        self.figure_size = []
        self.clusters_names = []
        self.clustnames=""
        for i in range(self.number_clusters):
            self.clusters_names.append(self.ledit_cluster_name[i].text())
            if i ==0:
                self.clustnames=f'{self.ledit_cluster_name[i].text()}'
            else:
                self.clustnames=self.clustnames+f';{self.ledit_cluster_name[i].text()}'
        print(self.clustnames)
        _ama.fill_clusters_names(self.clustnames)
        self.figure_size.append(self.left_ledit.text())
        self.figure_size.append(self.right_ledit.text())
        self.figure_size.append(self.bottom_ledit.text())
        self.figure_size.append(self.top_ledit.text())
        self.close()
    
    def get_layers_cwt(self):
        self.layers_altitude_cwt = ''
        for i in range(self.number_layers_cwt - 1):
            if i == 0:
                self.layers_altitude_cwt = self.altitudes_cwt[i].text()
                self.altitudes_cwt_value[i] = float(self.altitudes_cwt[i].text())
            else:
                self.layers_altitude_cwt = self.layers_altitude_cwt + ';' + self.altitudes_cwt[i].text()
                self.altitudes_cwt_value[i] = float(self.altitudes_cwt[i].text())
        print(self.layers_altitude_cwt)
        print("get_layers_cwt is started")
        self.alt_cwt_function = "Yes"
        self.close()

    def get_layers_pscf(self):
        self.layers_altitude_pscf = ''
        for i in range(self.number_layers_pscf - 1):
            if i == 0:
                self.layers_altitude_pscf = self.altitudes_pscf[i].text()
                self.altitudes_pscf_value[i] = float(self.altitudes_pscf[i].text())
            else:
                self.layers_altitude_pscf = self.layers_altitude_pscf + ';' + self.altitudes_pscf[i].text()
                self.altitudes_pscf_value[i] = float(self.altitudes_pscf[i].text())
        print(self.layers_altitude_pscf)
        print("get_layers_pscf is started")
        self.alt_pscf_function = "Yes"
        self.close()
        
    def get_weight_traj_cwt(self):
        self.nbr_traj_cwt = ''
        self.red_ratio_traj_cwt =''
        for i in range(self.number_layers_cwt):
            if i == 0:
                self.nbr_traj_cwt = self.weight_trajectories_cwt[i].text()
                self.red_ratio_traj_cwt = self.reduce_ratio_cwt[i].text()
            else:
                self.nbr_traj_cwt = self.nbr_traj_cwt + ':' + self.weight_trajectories_cwt[i].text()
                self.red_ratio_traj_cwt = self.red_ratio_traj_cwt + ':' + self.reduce_ratio_cwt[i].text()
        print("get_weight_traj_cwt is started")
        print(self.nbr_traj_cwt)
        print(self.red_ratio_traj_cwt)
        self.signal_weight_cwt.emit(str(self.nbr_traj_cwt))
        self.signal_reduce_cwt.emit(str(self.red_ratio_traj_cwt))
        self.close()

    def get_weight_endp_cwt(self):
        self.nbr_endp_cwt = ''
        self.red_ratio_endp_cwt = ''
        for i in range(self.number_layers_cwt):
            if i == 0:
                self.nbr_endp_cwt = self.weight_endpoints_cwt[i].text()
                self.red_ratio_endp_cwt = self.reduce_ratio_cwt[i].text()
            else:
                self.nbr_endp_cwt = self.nbr_endp_cwt + ':' + self.weight_endpoints_cwt[i].text()
                self.red_ratio_endp_cwt = self.red_ratio_endp_cwt + ':' + self.reduce_ratio_cwt[i].text()
        print("get_weight_endp_cwt is started")
        print(self.nbr_endp_cwt)
        print(self.red_ratio_endp_cwt)
        self.signal_weight_cwt.emit(str(self.nbr_endp_cwt))
        self.signal_reduce_cwt.emit(str(self.red_ratio_endp_cwt))
        self.close()

    # = = = = = = = = = =  = = = = = = = = = = = = = = = = = = = = = = = =
    def get_weight_traj_pscf(self):
        self.nbr_traj_pscf = ''
        self.red_ratio_traj_pscf =''
        for i in range(self.number_layers_pscf):
            if i == 0:
                self.nbr_traj_pscf = self.weight_trajectories_pscf[i].text()
                self.red_ratio_traj_pscf = self.reduce_ratio_pscf[i].text()
            else:
                self.nbr_traj_pscf = self.nbr_traj_pscf + ':' + self.weight_trajectories_pscf[i].text()
                self.red_ratio_traj_pscf = self.red_ratio_traj_pscf + ':' + self.reduce_ratio_pscf[i].text()
        print("get_weight_traj_pscf is started")
        print(self.nbr_traj_pscf)
        print(self.red_ratio_traj_pscf)
        self.signal_weight_pscf.emit(str(self.nbr_traj_pscf))
        self.signal_reduce_pscf.emit(str(self.red_ratio_traj_pscf))
        self.close()

    def get_weight_endp_pscf(self):
        self.nbr_endp_pscf = ''
        self.red_ratio_endp_pscf = ''
        for i in range(self.number_layers_pscf):
            if i == 0:
                self.nbr_endp_pscf = self.weight_endpoints_pscf[i].text()
                self.red_ratio_endp_pscf = self.reduce_ratio_pscf[i].text()
            else:
                self.nbr_endp_pscf = self.nbr_endp_pscf + ':' + self.weight_endpoints_pscf[i].text()
                self.red_ratio_endp_pscf = self.red_ratio_endp_pscf + ':' + self.reduce_ratio_pscf[i].text()
        print("get_weight_endp_pscf is started")
        print(self.nbr_endp_pscf)
        print(self.red_ratio_endp_pscf)
        self.signal_weight_pscf.emit(str(self.nbr_endp_pscf))
        self.signal_reduce_pscf.emit(str(self.red_ratio_endp_pscf))
        self.close()
        
    def clearLayout(self, layout):
        while layout.count():
            child = layout.takeAt(0)
            if child.widget():
                child.widget().deleteLater()

class Worker(QtCore.QObject):
    finished = QtCore.pyqtSignal()
    #progress = QtCore.pyqtSignal(int)
    def __init__(self, function = None, para1 = None, para2 = None, para3 = None, para4 = None, para5 = None, para6 = None, para7 = None, parent=None):
        QtCore.QThread.__init__(self, parent=parent)
        self.function= function
        self.para1 = para1
        self.para2 = para2
        self.para3 = para3
        self.para4 = para4
        self.para5 = para5
        self.para6 = para6
        self.para7 = para7

        

    def run(self):
        #self.thread_started = OutputGrabber(self.function, self.para1, self.para2, self.para3, self.para4, self.para5, self.para6, self.para7)
        #self.thread_started.start()
        #self.thread_started.mysignal.connect(self.append_text)
        if (self.function == "correlation"):
            _ama.correlation(float(self.para1), float(self.para2), float(self.para3), float(self.para4), float(self.para5), str(self.para6))
        self.finished.emit()


class Mainwindow(QtWidgets.QMainWindow):
    def __init__(self):
        super(Mainwindow, self).__init__()
        # Load the ui file
        uic.loadUi("../Interface/main.ui", self)
        self.showMaximized()                         # Opening the window in maximized size.
        # = = = = = = = = = = = = = =  home + back + display data  = = = = = = = = = = = = = = = = =
        self.btn_home.setIcon(QtGui.QIcon('../Icons/home.png'))
        self.btn_back.setIcon(QtGui.QIcon('../Icons/back.png'))
        self.btn_download_data.setIcon(QtGui.QIcon('../Icons/download.png'))
        self.btn_download_data.setIconSize(QtCore.QSize(int(self.btn_download_data.width()*0.75), int(self.btn_download_data.height()*0.7)))
        self.btn_generate_traj.setIcon(QtGui.QIcon('../Icons/traj_files.png'))
        self.btn_generate_traj.setIconSize(QtCore.QSize(int(self.btn_generate_traj.width()*0.75), int(self.btn_generate_traj.height()*0.7)))
        self.btn_input_data.setIcon(QtGui.QIcon('../Icons/import.png'))
        self.btn_input_data.setIconSize(QtCore.QSize(int(self.btn_input_data.width()*0.75), int(self.btn_input_data.height()*0.7)))
        self.btn_traj_analysis.setIcon(QtGui.QIcon('../Icons/analysis.png'))
        self.btn_traj_analysis.setIconSize(QtCore.QSize(int(self.btn_traj_analysis.width()*0.75), int(self.btn_traj_analysis.height()*0.7)))
        self.statusbar.showMessage("Open AMA: Radiations and Nuclear Systems group, Faculty of sciences, Abdelmalek Essaadi University, Tetouan, Morocco")
        # = = = = = = = = = = = = = =  stackedwidget functions  = = = = = = = = = = = = = = = = =
        self.stackedWidget_functions.setCurrentIndex(0)
        self.current_function.setText("* * Home * *")
        self.btn_home.clicked.connect(lambda x: self.stackedWidget_functions.setCurrentIndex(0))
        self.btn_home.clicked.connect(lambda x: self.current_function.setText("* * Home * *"))
        self.btn_generate_traj.clicked.connect(lambda x: self.stackedWidget_functions.setCurrentIndex(10))
        self.btn_generate_traj.clicked.connect(lambda x: self.current_function.setText("* * Generate Trajectories * *"))
        self.btn_download_data.clicked.connect(lambda x: self.stackedWidget_functions.setCurrentIndex(9))
        self.btn_download_data.clicked.connect(lambda x: self.current_function.setText("* * Download the archived meteorological data * *"))
        
        self.btn_input_data.clicked.connect(lambda x: self.stackedWidget_functions.setCurrentIndex(1))
        self.btn_input_data.clicked.connect(lambda x: self.current_function.setText("* * Input Data * *"))
        self.btn_traj_analysis.clicked.connect(lambda x: self.stackedWidget_functions.setCurrentIndex(2))
        self.btn_traj_analysis.clicked.connect(lambda x: self.current_function.setText("* * Analysis Methods * *"))
        self.btn_pscf_method.clicked.connect(lambda x: self.stackedWidget_functions.setCurrentIndex(3))
        self.btn_pscf_method.clicked.connect(lambda x: self.current_function.setText("* * PSCF Model * *"))
        self.btn_cwt_method.clicked.connect(lambda x: self.stackedWidget_functions.setCurrentIndex(4))
        self.btn_cwt_method.clicked.connect(lambda x: self.current_function.setText("* * CWT Model * *"))
        self.btn_clustering_method.clicked.connect(lambda x: self.stackedWidget_functions.setCurrentIndex(5))
        self.btn_clustering_method.clicked.connect(lambda x: self.current_function.setText("* * Clustering * *"))
        self.btn_correlation_method.clicked.connect(lambda x: self.stackedWidget_functions.setCurrentIndex(8))
        self.btn_correlation_method.clicked.connect(lambda x: self.current_function.setText("* * Correlation * *"))
        self.btn_kmean_method.clicked.connect(lambda x: self.stackedWidget_functions.setCurrentIndex(6))
        self.btn_kmean_method.clicked.connect(lambda x: self.current_function.setText("* * Clustering: Adaptive K-mean * *")) 
        self.btn_tsv_method.clicked.connect(lambda x: self.stackedWidget_functions.setCurrentIndex(7))
        self.btn_tsv_method.clicked.connect(lambda x: self.current_function.setText("* * Clustering: Total spatial minimum variance method * *"))
        self.btn_back_plot.clicked.connect(
            lambda x: self.stackedWidget_plot.setCurrentIndex(self.stackedWidget_plot.currentIndex()-1))
        self.btn_next_plot.clicked.connect(
            lambda x: self.stackedWidget_plot.setCurrentIndex(self.stackedWidget_plot.currentIndex()+1))
        self.btn_back.clicked.connect(self.back_stacks)
        self.btn_display_data.clicked.connect(lambda x: _ama.display_data())
        #================================ layers==========================
        self.layers = Create_subWindow(self.sBox_layers_pscf.value())
        self.layers.signal_reduce_cwt.connect(self.red_append_cwt)
        self.layers.signal_weight_cwt.connect(self.weight_append_cwt)
        self.layers.signal_reduce_pscf.connect(self.red_append_pscf)
        self.layers.signal_weight_pscf.connect(self.weight_append_pscf)
        self.layers.signal_roseplot_groups.connect(self.number_groups_roseplot)
        self.layers.signal_plot_roseplot.connect(self.receive_signal_to_plot_roseplot)
        # = = = = = = = = = matplotlib widge = = = = = = = = = = = = = = = = = = = = = = =
        self.btn_plot_cwt.clicked.connect(lambda x: self.stacked_plot(self.layers.number_layers_cwt, self.cwt_plot))
        self.btn_plot_pscf.clicked.connect(lambda x: self.stacked_plot(self.layers.number_layers_pscf, self.pscf_plot))
        self.btn_plot_initial.clicked.connect(lambda x: self.stacked_plot(1, "initial"))
        self.stacked_plot(1, "initial")
        # = = = = = = = = = = = = = = = = = = = = = = Tab_Input_data = = = = = = = = = = = = = = = = = = = = = = =
        self.btn_open_traj_folder.clicked.connect(lambda x : self.openTrajectoriesFolder("traj_folder"))
        self.btn_copy_folder.clicked.connect(lambda x : self.openTrajectoriesFolder("copy_folder"))
        self.btn_open_data_file.clicked.connect(lambda x : self.openDataFile())
        self.btn_read_traj.clicked.connect(lambda x: self.start_threading("read_traj",self.ledit_traj_path.text()))
        self.btn_clear_traj.clicked.connect(lambda x: self.are_you_sure("clear_traj"))
        self.btn_read_data.clicked.connect(lambda x: self.start_threading("read_data", self.ledit_data_path.text()))
        self.btn_clear_data.clicked.connect(lambda x: self.are_you_sure("clear_data"))
        self.btn_date_convert_begin.clicked.connect(lambda x: self.date_format("begin"))
        self.btn_date_convert_end.clicked.connect(lambda x: self.date_format("end"))
        self.btn_add_data_traj.clicked.connect(lambda x: self.add_data_to_trajectories())
        """self.btn_add_data_traj.clicked.connect(lambda x: _ama.get_index_data_headers(self.cBox_data.currentText(), "concentration"))
        self.btn_add_data_traj.clicked.connect(lambda x: self.start_threading("ctr_trajs", self.ledit_missing_values.text()))"""
        self.btn_copy.clicked.connect(lambda x: self.start_threading("select_traj", self.ledit_min_sel.text(), self.ledit_max_sel.text(), self.ledit_copy_path.text()))
        # ========================== Tab_Trajectories_analysis ===========================
                                # * * * * * * * * Tab_CWT * * * * * * * *
        # * * *
        self.mode_cwt = "2D_CWT"
        self.sBox_layers_cwt.setReadOnly(True)
        self.sBox_layers_cwt.setStyleSheet('background-color:#A9B4B2')
        self.ledit_weight_cwt.setReadOnly(False)
        self.ledit_reduce_ratio_cwt.setReadOnly(False)
        self.btn_create_layers_cwt.hide()
        self.btn_create_weights_cwt.hide()
        # * * *
        self.rightcwt = 0
        self.leftcwt = 0
        self.topcwt = 0
        self.bottomcwt = 0
        self.sizecellcwt = 0
        # * * * 
        self.btn_rd_2D_cwt.toggled.connect(lambda x: self.radio("2D_CWT"))
        self.btn_rd_3D_cwt.toggled.connect(lambda x: self.radio("3D_CWT"))
        self.btn_rd_trajectory_cwt.toggled.connect(lambda x: self.radio("trajectory_CWT"))
        self.btn_rd_endpoint_cwt.toggled.connect(lambda x: self.radio("endpoints_CWT"))
        self.cwt_plot = "WCWT"
        self.btn_rd_cwt.toggled.connect(lambda x: self.radio("plot_cwt"))
        self.btn_rd_wcwt.toggled.connect(lambda x: self.radio("plot_wcwt"))
        self.weight_CWT = "endpoints"
        self.sBox_layers_cwt.setReadOnly(True)
        self.sBox_layers_cwt.setStyleSheet('background-color:#A9B4B2')
        self.btn_cwt.clicked.connect(lambda x: self.cwt("CWT"))
        self.btn_wcwt.clicked.connect(lambda x: self.cwt("WCWT"))
        self.btn_create_layers_cwt.clicked.connect(lambda x: self.layers.layers_alt_cwt(self.sBox_layers_cwt.value()))
        # self.btn_plot_cwt.clicked.connect(lambda x: self.plot_cwt())
                                    # * * * * * * Tab_PSCF * * * * * *
        # * * *
        self.mode_pscf = "2D_PSCF"
        self.sBox_layers_pscf.setReadOnly(True)
        self.sBox_layers_pscf.setStyleSheet('background-color:#A9B4B2')
        self.ledit_weight_pscf.setReadOnly(False)
        self.ledit_reduce_ratio_pscf.setReadOnly(False)
        self.btn_create_layers_pscf.hide()
        self.btn_create_weights_pscf.hide()
        # * * *
        self.rightpscf=0
        self.leftpscf = 0
        self.toppscf = 0
        self.bottompscf= 0
        self.sizecellpscf= 0
        # * * * 
        self.btn_rd_2D_pscf.toggled.connect(lambda x: self.radio("2D_PSCF"))
        self.btn_rd_3D_pscf.toggled.connect(lambda x: self.radio("3D_PSCF"))
        self.btn_rd_trajectory_pscf.toggled.connect(lambda x: self.radio("trajectory_PSCF"))
        self.btn_rd_endpoint_pscf.toggled.connect(lambda x: self.radio("endpoints_PSCF"))
        self.btn_rd_pscf.toggled.connect(lambda x: self.radio("plot_pscf"))
        self.btn_rd_wpscf.toggled.connect(lambda x: self.radio("plot_wpscf"))
        self.pscf_plot = "WPSCF"
        self.weight_PSCF = "endpoints"
        self.btn_pscf.clicked.connect(lambda x: self.pscf("PSCF"))
        self.btn_wpscf.clicked.connect(lambda x: self.pscf("WPSCF"))
        self.btn_create_layers_pscf.clicked.connect(lambda x: self.layers.layers_alt_pscf(self.sBox_layers_pscf.value()))
        self.btn_create_weights_cwt.clicked.connect(lambda x: self.layers.layers_weight_endp_cwt())
        self.btn_create_weights_pscf.clicked.connect(lambda x: self.layers.layers_weight_endp_pscf())

        # * * * * * * * * * * * * * * * * * * Clustering * * * * * * * * * * * * * * * * * * * * * *
            # K-mean * * * * * *
        self.dir_clustering = "" 
        self.cBox_clustering_results.addItem("Add clustering results ...")
        self.btn_start_clst_kmean.clicked.connect(lambda x: self.clustering("kmean"))
        self.btn_plot_tsv_kmean.clicked.connect(lambda x: self.stacked_plot(1, "var_tsv_kmean"))
        self.btn_choice_clusters_kmean.clicked.connect(lambda x: self.choice_nbr_clusters("kmean"))
        self.btn_plot_mean_clst_kmean.clicked.connect(lambda x: self.stacked_plot(1, "clustering_kmean"))
        self.btn_figure_kmean.clicked.connect(lambda x: self.figure_size("kmean"))
        self.btn_add_clusters_data_kmean.clicked.connect(lambda x: _ama.add_clusters_to_data_file("kmean", self.dir_clustering))
        self.btn_freq_clst_kmean.clicked.connect(lambda x: self.stacked_plot(1, "freq_kmean"))
        self.btn_roseplot_kmean.clicked.connect(lambda x: self.layers.roseplot("kmean", self.dir_clustering))
        self.btn_remove_clusters_from_data_kmean.clicked.connect(lambda x: _ama.remove_columns_data("clusters"))
        self.nbr_clusters_kmean = 1
        self.btn_clear_clst_kmean.clicked.connect(lambda x: _ama.clear_clusters())
        self.btn_select_clustering.clicked.connect(lambda x: self.load_clusters())
        
        
        
            # TSV * * * * * *
        self.btn_start_clst_tsv.clicked.connect(lambda x: self.clustering("TSV"))
        self.btn_plot_tsv_tsv.clicked.connect(lambda x: self.stacked_plot(1, "var_tsv_tsv"))
        self.btn_choice_clusters_tsv.clicked.connect(lambda x: self.choice_nbr_clusters("TSV"))
        self.btn_plot_mean_clst_tsv.clicked.connect(lambda x: self.stacked_plot(1, "clustering_TSV"))
        self.btn_figure_tsv.clicked.connect(lambda x: self.figure_size("TSV"))
        self.btn_add_clusters_data_tsv.clicked.connect(lambda x: _ama.add_clusters_to_data_file("TSV", ""))
        self.btn_freq_clst_tsv.clicked.connect(lambda x: self.stacked_plot(1, "freq_tsv"))
        self.btn_roseplot_tsv.clicked.connect(lambda x: self.layers.roseplot("TSV"))
        self.btn_remove_clusters_from_data_tsv.clicked.connect(lambda x: _ama.remove_columns_data("clusters"))
        self.nbr_clusters_tsv = 1
        self.btn_clear_clst_tsv.clicked.connect(lambda x: _ama.clear_clusters())
        
        
        """self.btn_load_clusters_kmean.clicked.connect(lambda x: _ama.open_clustering("kmean"))"""
        
        
        
        # Correlation * * * * * * *
        self.leftcorr = 0
        self.rightcorr = 0
        self.bottomcorr = 0
        self.topcorr = 0
        self.sizecellcorr = 0
        self.btn_correlation.clicked.connect(lambda x: self.correlation_traj())
        self.btn_plot_correlation.clicked.connect(lambda x: self.stacked_plot(1, "correlation_traj"))
        
        self.on_icon = QtGui.QPixmap('../Icons/red_dot.png').scaled(int(self.btn_home.width()/4),int(self.btn_home.height()/4))
        self.lab_icons_working.setPixmap(self.on_icon)
        
        # Auto_traj
        self.grids_paths = [None]
        self.grids_paths_sorted = [None]
        self.grids_files = [None]
        self.grids_files_sorted = [None]
        self.btn_add_grid.clicked.connect(lambda x: self.add_grids())
        self.btn_delete_grid.clicked.connect(lambda x: self.lWid_grid.takeItem(self.lWid_grid.currentRow()))
        self.btn_delete_all_grid.clicked.connect(lambda x: self.lWid_grid.clear())
        self.select_dir_generate_traj.clicked.connect(lambda x : self.openTrajectoriesFolder("output_traj_folder"))
        self.btn_start_gen_traj.clicked.connect(lambda x : self.run_hysplit())
        self.btn_exec.clicked.connect(lambda x : self.openTrajectoriesFolder("exec_folder"))
        
        # Download Meteorological data
        self.btn_download_outputdir.clicked.connect(lambda x : self.openTrajectoriesFolder("output_download_meteorological_parameters"))
        self.btn_download.clicked.connect(lambda x : self.download_data())
        
        
        self.show()
    
    def dir_clust_results(self):
        self.cBox_clustering_results.clear()
        if os.path.isfile("../Results/Cluster/list_dir_clustering"): 
            with open("../Results/Cluster/list_dir_clustering") as file:
                for line in file:
                    self.cBox_clustering_results.addItem(line.rstrip())
        self.cBox_clustering_results.addItem("Others clustering results...")
    
    def load_clusters(self):
        if(self.cBox_clustering_results.currentText() == "Others clustering results..." or self.cBox_clustering_results.currentText() == "Add clustering results ..."):
            self.openTrajectoriesFolder("load_clusters_kmean")
            self.dir_clustering = self.cBox_clustering_results.currentText()
            _ama.open_clustering("kmean", self.cBox_clustering_results.currentText())
             
        else:
            try:
                self.dir_clustering = self.cBox_clustering_results.currentText()
                _ama.open_clustering("kmean", self.cBox_clustering_results.currentText())
            except Exception as e:
                self.append_text(str(e))
    
    def add_data_to_trajectories(self):
        _ama.get_index_data_headers(self.cBox_data.currentText(), "concentration")
        self.start_threading("ctr_trajs", self.ledit_missing_values.text())
        
    def add_grids(self):
        filenames, _ = QtWidgets.QFileDialog.getOpenFileNames(None, "QtWidgets.QFileDialog.getOpenFileNames()", "", "All Files (*)")
        if filenames:
            for filename in filenames:
                self.lWid_grid.addItem(filename)
        self.sortGrids()

    def sortGrids(self):
        months = {'jan':1, 'feb':2, 'mar':3, 'apr':4, 'may':5, 'jun':6, 'jul':7, 'aug':8, 'sep':9, 'oct':10, 'nov':11, 'dec':12}
        self.grids_paths = [None] * self.lWid_grid.count()
        self.grids_paths_sorted = []
        self.grids_files = [None] * self.lWid_grid.count()
        self.grids_files_sorted = []
        for i in range(self.lWid_grid.count()):
            self.grids_paths[i] = self.lWid_grid.item(i).text()
            self.grids_files[i] = self.grids_paths[i][self.grids_paths[i].rfind('/')+1:]
                #    gdas1.feb06.w2
        while(len(self.grids_files)>0):
            ind_min = 0
            minn = self.grids_files[ind_min]
            min_w = minn[minn.find('.') + 8]
            min_m = minn[minn.find('.') + 1 : minn.find('.') + 4]
            min_y = minn[minn.find('.') + 4 : minn.find('.') + 6]
            for i in range(len(self.grids_files)):
                if (int(min_y)> int(self.grids_files[i][self.grids_files[i].find('.') + 4 : self.grids_files[i].find('.') + 6])):
                    ind_min = i
                    minn = self.grids_files[ind_min]
                    min_w = minn[minn.find('.') + 8]
                    min_m = minn[minn.find('.') + 1 : minn.find('.') + 4]
                    min_y = minn[minn.find('.') + 4 : minn.find('.') + 6]
                elif (int(min_y) == int(self.grids_files[i][self.grids_files[i].find('.') + 4 : self.grids_files[i].find('.') + 6]) and int(months.get(min_m)) > int(months.get(self.grids_files[i][self.grids_files[i].find('.') + 1 : self.grids_files[i].find('.') + 4]))):
                    ind_min = i
                    minn = self.grids_files[ind_min]
                    min_w = minn[minn.find('.')+8]
                    min_m = minn[minn.find('.')+1 : minn.find('.') + 4]
                    min_y = minn[minn.find('.')+4 : minn.find('.') + 6]
                elif (int(min_y) == int(self.grids_files[i][self.grids_files[i].find('.')+4 : self.grids_files[i].find('.')+6]) and int(months.get(min_m)) == int(months.get(self.grids_files[i][self.grids_files[i].find('.')+1 : self.grids_files[i].find('.')+4])) and int(min_w) > int(self.grids_files[i][self.grids_files[i].find('.')+8])):
                    ind_min = i
                    minn = self.grids_files[ind_min]
                    min_w = minn[minn.find('.')+8]
                    min_m = minn[minn.find('.')+1 : minn.find('.')+4]
                    min_y = minn[minn.find('.')+4 : minn.find('.')+6]
            # if(self.grids_paths[ind_min] is exist in grids_paths_sorted then append)
            self.grids_paths_sorted.append(self.grids_paths[ind_min])
            self.grids_files_sorted.append(self.grids_files[ind_min])
            del self.grids_paths[ind_min]
            del self.grids_files[ind_min]
        self.lWid_grid.clear()
        for filename in self.grids_paths_sorted:
            self.lWid_grid.addItem(filename)
    
    
    

    def run_hysplit(self): 
        months = {'jan':1, 'feb':2, 'mar':3, 'apr':4, 'may':5, 'jun':6, 'jul':7, 'aug':8, 'sep':9, 'oct':10, 'nov':11, 'dec':12}
        weeks = {1:"w1",2:"w2", 3:"w3", 4:"w4", 5:"w5"}
        year_s = self.sBox_year_start.value()
        month_s = self.sBox_month_start.value()
        day_s = self.sBox_day_start.value()
        hour_s = self.sBox_hour_start.value()
        year_e = self.sBox_year_end.value()
        month_e = self.sBox_month_end.value()
        day_e = self.sBox_day_end.value()
        hour_e = self.sBox_hour_end.value()
        every_h = self.sBox_every.value()
        ntraj = self.sBox_number_starting_locations.value()
        lat = self.sBox_lat.value()
        lon = self.sBox_lon.value()
        alt = self.sBox_alt.value()
        length = self.sBox_length_traj.value()
        vertical = self.sBox_vertical.value()
        top_hight = self.sBox_top_hight.value()
        path_exec = self.ledit_exec_path.text()
        
        out_dir = self.ledit_output_traj.text()+"/"
        out_file = self.ledit_filenames.text()
        
        date_start = '{:0>4}'.format(year_s)+'-{:0>2}'.format(month_s)+'-{:0>2}'.format(day_s)+' {:0>2}'.format(hour_s)+':00'
        date_end = '{:0>4}'.format(year_e)+'-{:0>2}'.format(month_e)+'-{:0>2}'.format(day_e)+' {:0>2}'.format(hour_e)+':00'
        start = datetime.datetime.strptime(date_start, '%Y-%m-%d %H:%M')
        end = datetime.datetime.strptime(date_end, '%Y-%m-%d %H:%M')
        
        step = datetime.timedelta(hours=every_h)
        length1 = datetime.timedelta(hours=length)
        
        while(start <= end):
            cmd = ''
            data = []
            x=start+length1
            week=""
            month=x.month
            year=x.year
            if (x.day<=7):
                week="w1"
            elif (x.day<=14):
                week="w2"
            elif (x.day<=21):
                week="w3"
            elif (x.day<=28):
                week="w4"
            elif (x.day>28):
                week="w5"
            data1="gdas1."+list(months.keys())[list(months.values()).index(x.month)]+'{:0>2}'.format(int(str(x.year)[-2:]))+"."+week
            data.append(data1)
            ww=""
            if (start.day<=7):
                ww="w1"
            elif (start.day<=14):
                ww="w2"
            elif (start.day<=21):
                ww="w3"
            elif (start.day<=28):
                ww="w4"
            elif(start.day>28):
                ww="w5"
            data2="gdas1."+list(months.keys())[list(months.values()).index(start.month)]+'{:0>2}'.format(int(str(start.year)[-2:]))+"."+ww
            while( data1 != data2):
                if (week == "w5"):
                    week = "w1"
                    x  += relativedelta(months=1) 
                elif (week =="w4"):
                    if (month == 2):
                        if (year%4==0):
                            week = "w5"
                        else:
                            week = "w1"
                            x  += relativedelta(months=1) 
                    else:
                        week="w5"
                else:
                    week = weeks.get(list(weeks.keys())[list(weeks.values()).index(week)+1])
                    #week++
                data1="gdas1."+list(months.keys())[list(months.values()).index(x.month)]+'{:0>2}'.format(int(str(x.year)[-2:]))+"."+week
                data.append(data1)
            cmd ='./traj_expect_hyts_std.sh '+ path_exec +"/hyts_std "+'{:0>2} '.format(int(str(start.year)[-2:]))+ '{:0>2} '.format(start.month)+ '{:0>2} '.format(start.day)+ '{:0>2} '.format(start.hour)+ str(ntraj)+" " + str(lat)+" "+ str(lon)+" "+str(alt)+" "+ str(length)+" "+ str(vertical)+" "+ str(top_hight)+" "+str(len(data))+" "+ out_dir+" "+ out_file
            for i in range(len(data)):
                cmd = cmd + " "+ self.grids_paths_sorted[ self.grids_files_sorted.index(str(data[i]))][:-14] + " "+ data[i]
            print("_____________________________________________________________________________")
            os.system(cmd)
            start += step
           
    def download_data(self):
        months = {'jan':1, 'feb':2, 'mar':3, 'apr':4, 'may':5, 'jun':6, 'jul':7, 'aug':8, 'sep':9, 'oct':10, 'nov':11, 'dec':12}
        weeks = {1:"w1",2:"w2", 3:"w3", 4:"w4", 5:"w5"}
        
        year_s = self.sBox_download_year_start.value()
        month_s = self.sBox_download_month_start.value()
        day_s = self.sBox_download_day_start.value()
        hour_s = self.sBox_download_hour_start.value()
        year_e = self.sBox_download_year_end.value()
        month_e = self.sBox_download_month_end.value()
        day_e = self.sBox_download_day_end.value()
        hour_e = self.sBox_download_hour_end.value()
        outdir = self.ledit_download.text()
        date_start = '{:0>4}'.format(year_s)+'-{:0>2}'.format(month_s)+'-{:0>2}'.format(day_s)+' {:0>2}'.format(hour_s)+':00'
        date_end = '{:0>4}'.format(year_e)+'-{:0>2}'.format(month_e)+'-{:0>2}'.format(day_e)+' {:0>2}'.format(hour_e)+':00'
        date_start = '{:0>4}'.format(year_s)+'-{:0>2}'.format(month_s)+'-{:0>2}'.format(day_s)+' {:0>2}'.format(hour_s)+':00'
        date_end = '{:0>4}'.format(year_e)+'-{:0>2}'.format(month_e)+'-{:0>2}'.format(day_e)+' {:0>2}'.format(hour_e)+':00'
        start = datetime.datetime.strptime(date_start, '%Y-%m-%d %H:%M')
        end = datetime.datetime.strptime(date_end, '%Y-%m-%d %H:%M')
        
        
        if (self.cBox_download_data.currentText() == "GDAS one-degree archive (Dec 2004 - present)"):
            week = ""
            if (day_e<=7):
                week="w1"
            elif (day_e<=14):
                week="w2"
            elif (day_e<=21):
                week="w3"
            elif (day_e<=28):
                week="w4"
            elif (day_e>28):
                week="w5"
            data2="gdas1."+list(months.keys())[list(months.values()).index(month_e)]+'{:0>2}'.format(int(str(year_e)[-2:]))+"."+week
        
        
            week = ""
            if (day_s<=7):
                week="w1"
            elif (day_s<=14):
                week="w2"
            elif (day_s<=21):
                week="w3"
            elif (day_s<=28):
                week="w4"
            elif (day_s>28):
                week="w5"
            data1="gdas1."+list(months.keys())[list(months.values()).index(month_s)]+'{:0>2}'.format(int(str(year_s)[-2:]))+"."+week
            #==============================================================================================================
        
            if(start<=end):
                while( data1 != data2):
                    if (week == "w5"):
                        week = "w1"
                        if (month_s ==12):
                            month_s = 1
                            year_s = year_s+1
                        else:
                            month_s=month_s+1
                    elif (week =="w4"):
                        if (month_s== 2):
                            if (year_s%4==0):
                                week = "w5"
                            else:
                                week = "w1"
                                month_s=month_s+1
                        else:
                            week="w5"
                    else:
                        week = weeks.get(list(weeks.keys())[list(weeks.values()).index(week)+1])
                    data1="gdas1."+list(months.keys())[list(months.values()).index(month_s)]+'{:0>2}'.format(int(str(year_s)[-2:]))+"."+week
                    link = 'ftp://ftp.arl.noaa.gov/pub/archives/gdas1/'+data1
                    fiile=wget.download(link,outdir)
                    fiile=""
        elif(self.cBox_download_data.currentText() == "NCEP/NCAR Reanalysis (1948 - present)"):
            while(start <= end):
                link = "ftp://ftp.arl.noaa.gov/pub/archives/reanalysis/RP" + '{}'.format(start.year)+'{:0>2}'.format(start.month)+ ".gbl"
                fiile = wget.download(link, outdir)
                fiile = ""
                start += relativedelta(months=1)
                   
    def correlation_traj(self):
        self.leftcorr = float(self.ledit_left_cwt_correlation.text())
        self.rightcorr = float(self.ledit_right_correlation.text())
        self.bottomcorr = float(self.ledit_bottom_correlation.text())
        self.topcorr = float(self.ledit_top_correlation.text())
        self.sizecellcorr = float(self.ledit_cell_size_correlation.text())
        self.start_threading("correlation", self.ledit_left_cwt_correlation.text(),
                                            self.ledit_right_correlation.text(), self.ledit_bottom_correlation.text(),
                                            self.ledit_top_correlation.text(), (self.ledit_cell_size_correlation.text()),
                                            self.cBox_meteorological_parameters_correlation.currentText())
                                            
    def plot_correlation_traj(self,figure,canvas):
        self.mode_correlation = "2D" ## Temporarily
        """if self.mode_correlation == "3D":
            #self.titel_correlation = f'Correlation #{i}'
            self.correlation_file = pd.read_table(f'../Results/Correlation/correlation{i}.csv', sep=";")
            self.y_list = self.correlation_file['lon']
            self.x_list = self.correlation_file['lat']
            self.z_list = self.correlation_file['correlation']
            self.number_of_endpoints_in_cells = self.correlation_file['number_of_endpoints_in_cells']"""
        
            
        #elif self.mode_correlation == "2D":
        self.titel_correlation = 'Correlation'
        self.correlation_file = pd.read_table('../Results/Correlation/correlation.csv', sep=";")
        self.y_list = self.correlation_file['lon']
        self.x_list = self.correlation_file['lat']
        self.z_list = self.correlation_file['correlation']
        self.number_of_endpoints_in_cells = self.correlation_file['number_of_endpoints_in_cells']
        self.z = self.z_list.to_numpy()
        for i in range (len(self.number_of_endpoints_in_cells)):
            if (self.number_of_endpoints_in_cells[i] < int(self.sBox_nbr_paires_correlation.text())):
                self.z[i]=0
        self.mx = max(self.z_list)
        self.mn = min(self.z_list)
        self.r = self.mx - self.mn
        
        self.heatmap = np.reshape(self.z, (int((self.topcorr - self.bottomcorr)/self.sizecellcorr) ,int((self.rightcorr-self.leftcorr)/self.sizecellcorr)))
        self.heatmap[self.heatmap == 0] = np.nan
        
        cmap = plt.get_cmap(self.cBox_colorbar_correlation.currentText(), self.sBox_bounds_correlation.value())
        # = = = = = = = = = = = = = =  map = = = = = = = = = =
        ax = figure.add_subplot(1, 1, 1, projection=ccrs.PlateCarree())
        ax.set_extent([min(self.x_list), max(self.x_list), min(self.y_list), max(self.y_list)], crs=ccrs.PlateCarree())
        # ax.add_feature(cfeature.LAND)
        # ax.add_feature(cfeature.OCEAN)
        ax.add_feature(cfeature.COASTLINE)
        ax.add_feature(cfeature.BORDERS, linestyle=':')
        ax.gridlines(draw_labels=True, dms=True, x_inline=False, y_inline=False, )
        # ax.add_feature(cfeature.LAKES, alpha=0.5)
        # ax.add_feature(cfeature.RIVERS)cwt
        # ======================================= data plot
        # =======================================
        
        
        figure = plt.imshow(self.heatmap, extent=[min(self.x_list), max(self.x_list), min(self.y_list), max(self.y_list)],
                            interpolation='none', origin="lower", cmap=cmap)
        plt.colorbar(figure, cmap=cmap, fraction=0.030, pad=0.1)
        plt.title(self.titel_correlation)
        canvas.draw()
        
    def receive_signal_to_plot_roseplot(self, text):
        if text == "kmean":
            self.stacked_plot(1, "rose_kmean")
        elif text =="TSV":
            self.stacked_plot(1, "rose_tsv")

    def date_format(self,date):
        if (date == "begin"):
            _ama.get_index_data_headers(self.cBox_date_begin.currentText(), "date_begin")
            print("<<<<<<<<<<<<<<<<<<<<<<")
            self.start_threading("date_format", "begin", self.cBox_date_format_begin.currentText())
        elif (date == "end"):
            _ama.get_index_data_headers(self.cBox_date_end.currentText(), "date_end")
            self.start_threading("date_format", "end", self.cBox_date_format_end.currentText())
        
    
    
    def openTrajectoriesFolder(self,function):
        if function == "traj_folder":
            directory = str(QtWidgets.QFileDialog.getExistingDirectory())
            self.ledit_traj_path.setText('{}'.format(directory))  
        elif function == "copy_folder":
            directory = str(QtWidgets.QFileDialog.getExistingDirectory())
            self.ledit_copy_path.setText('{}'.format(directory))
        elif function == "output_traj_folder":
            directory = str(QtWidgets.QFileDialog.getExistingDirectory())
            self.ledit_output_traj.setText('{}'.format(directory))
        elif function == "exec_folder":
            directory = str(QtWidgets.QFileDialog.getExistingDirectory())
            self.ledit_exec_path.setText('{}'.format(directory))
        elif function == "output_download_meteorological_parameters":
            directory = str(QtWidgets.QFileDialog.getExistingDirectory())
            self.ledit_download.setText('{}'.format(directory))
        elif function == "load_clusters_kmean":
            directory = str(QtWidgets.QFileDialog.getExistingDirectory())
            self.cBox_clustering_results.addItem('{}'.format(directory))
            self.cBox_clustering_results.setCurrentText('{}'.format(directory))
           
        
        
        
            
    def openDataFile(self):
        filepath = QtWidgets.QFileDialog.getOpenFileName(self, 'Hey! Select a File',"","csv files (*.csv)")
        self.ledit_data_path.setText('{}'.format(filepath[0]))
        


    def number_groups_roseplot(self,text):
        strin =text
        x = strin.split(";")
        self.nbr_groups =0
        for i in range(len(x)):
            self.nbr_groups=self.nbr_groups+1

    def red_append_cwt(self, text):
        self.ledit_reduce_ratio_cwt.setText(text)

    def weight_append_cwt(self, text):
        self.ledit_weight_cwt.setText(text)

    def red_append_pscf(self, text):
        self.ledit_reduce_ratio_pscf.setText(text)

    def weight_append_pscf(self, text):
        self.ledit_weight_pscf.setText(text)

    def back_stacks(self):
        if (self.stackedWidget_functions.currentIndex() == 1 or self.stackedWidget_functions.currentIndex() == 2 ):
            self.stackedWidget_functions.setCurrentIndex(0)
            self.current_function.setText(" ")
        elif (self.stackedWidget_functions.currentIndex() == 3 or self.stackedWidget_functions.currentIndex() == 4 or self.stackedWidget_functions.currentIndex() == 5 or self.stackedWidget_functions.currentIndex() == 8 ):
            self.stackedWidget_functions.setCurrentIndex(2)
            self.current_function.setText(" ")
        elif(self.stackedWidget_functions.currentIndex() == 6 or self.stackedWidget_functions.currentIndex()== 7):
            self.stackedWidget_functions.setCurrentIndex(5)
            self.current_function.setText(" ")

    def stacked_plot(self, nbr_plots, function):
        self.clearLayout(self.lay_plot_parent)
        self.stackedWidget_plot = QtWidgets.QStackedWidget()
        if nbr_plots == 1:
            self.btn_back_plot.setEnabled(False)
            self.btn_next_plot.setEnabled(False)
        else:
            self.btn_back_plot.setEnabled(True)
            self.btn_next_plot.setEnabled(True)
        self.lay_plot_parent.addWidget(self.stackedWidget_plot)
        self.stacks = [None] * nbr_plots
        self.figures = [None] * nbr_plots
        self.canvass = [None] * nbr_plots
        self.toolbars = [None] * nbr_plots
        for i in range(nbr_plots):
            self.stacks[i] = QtWidgets.QWidget()
            self.figures[i] = plt.figure(figsize=(19.20,10.80), tight_layout=True)
            self.canvass[i] = FigureCanvas(self.figures[i])
            self.toolbars[i] = NavigationToolbar(self.canvass[i], self)
            self.stack_i(i, self.toolbars[i], self.canvass[i])
            self.stackedWidget_plot.addWidget(self.stacks[i])
            if (function == "CWT"):
                if nbr_plots == 1:
                    self.plot_cwt(i, function, self.figures[i], self.canvass[i])
                else:
                    self.plot_cwt(i, function, self.figures[i], self.canvass[i])
            elif (function == "WCWT"):
                if nbr_plots == 1:
                    self.plot_cwt(i, function, self.figures[i], self.canvass[i])
                else:
                    self.plot_cwt(i, function, self.figures[i], self.canvass[i])
            elif(function == "PSCF"):
                if nbr_plots == 1:
                    self.plot_pcsf(i, function, self.figures[i], self.canvass[i])
                else:
                    self.plot_pcsf(i, function, self.figures[i], self.canvass[i])
            elif (function == "WPSCF"):
                if nbr_plots == 1:
                    self.plot_pcsf(i, function, self.figures[i], self.canvass[i])
                else:
                    self.plot_pcsf(i, function, self.figures[i], self.canvass[i])
            elif(function == "initial"):
                self.plot_initial(self.figures[i], self.canvass[i])
            elif(function == "var_tsv_kmean"):
                self.plot_var_TSV(self.figures[i], self.canvass[i],"kmean")
            elif (function == "var_tsv_tsv"):
                self.plot_var_TSV(self.figures[i], self.canvass[i], "TSV")
            elif (function == "clustering_kmean"):
                self.plot_clustering(self.figures[i], self.canvass[i], "kmean")
            elif (function == "clustering_TSV"):
                self.plot_clustering(self.figures[i], self.canvass[i], "TSV")
            elif(function == "freq_tsv"):
                self.frequency(self.figures[i], self.canvass[i],"TSV")
            elif (function == "freq_kmean"):
                self.frequency(self.figures[i], self.canvass[i], "kmean")
            elif (function == "rose_tsv"):
                self.rose_plot(self.figures[i], self.canvass[i], "TSV")
            elif (function == "rose_kmean"):
                self.rose_plot(self.figures[i], self.canvass[i], "kmean")
            elif (function =="correlation_traj"):
                self.plot_correlation_traj(self.figures[i], self.canvass[i])



    def stack_i(self, i, widget1=None, widget2=None, widget3=None):
        layout = QtWidgets.QVBoxLayout()
        layout.addWidget(widget1)
        layout.addWidget(widget2)
        self.stacks[i].setLayout(layout)

    def display(self, i):
        self.stackedWidget_plot.setCurrentIndex(i)

    def clearLayout(self, layout):
        while layout.count():
            child = layout.takeAt(0)
            if child.widget():
                child.widget().deleteLater()

    def cwt(self, function):
        self.rightcwt = float(self.ledit_right_cwt.text())
        self.leftcwt = float(self.ledit_left_cwt.text())
        self.topcwt = float(self.ledit_top_cwt.text())
        self.bottomcwt = float(self.ledit_bottom_cwt.text())
        self.sizecellcwt = float(self.ledit_cell_size_cwt.text())
        if function == "CWT":
            if self.mode_cwt == "2D_CWT":
                self.start_threading("2D_CWT", self.ledit_left_cwt.text(), self.ledit_right_cwt.text(),
                                     self.ledit_bottom_cwt.text(), self.ledit_top_cwt.text(),
                                     self.ledit_cell_size_cwt.text())

            elif self.mode_cwt == "3D_CWT":
                self.start_threading("3D_CWT", self.ledit_left_cwt.text(), self.ledit_right_cwt.text(),
                                     self.ledit_bottom_cwt.text(), self.ledit_top_cwt.text(),
                                     self.ledit_cell_size_cwt.text(), self.layers.layers_altitude_cwt)

        elif function == "WCWT":
            if self.mode_cwt == "2D_CWT":
                if self.weight_CWT == "endpoints":
                    self.start_threading("2D_WCWT", self.ledit_reduce_ratio_cwt.text(), self.ledit_weight_cwt.text(),
                                         self.ledit_left_cwt.text(), self.ledit_bottom_cwt.text(),
                                         self.ledit_cell_size_cwt.text(), "endpoints")

                elif self.weight_CWT == "trajectory":
                    self.start_threading("2D_WCWT", self.ledit_reduce_ratio_cwt.text(), self.ledit_weight_cwt.text(),
                                         self.ledit_left_cwt.text(), self.ledit_bottom_cwt.text(),
                                         self.ledit_cell_size_cwt.text(), "trajectory")

            elif self.mode_cwt == "3D_CWT":
                if self.weight_CWT == "endpoints":
                    self.start_threading("3D_WCWT", self.layers.red_ratio_endp_cwt, self.layers.nbr_endp_cwt,
                                         self.ledit_left_cwt.text(), self.ledit_bottom_cwt.text(),
                                         self.ledit_cell_size_cwt.text(), "endpoints")

                elif self.weight_CWT == "trajectory":
                    self.start_threading("3D_WCWT", self.layers.red_ratio_traj_cwt, self.layers.nbr_traj_cwt,
                                         self.ledit_left_cwt.text(), self.ledit_bottom_cwt.text(),
                                         self.ledit_cell_size_cwt.text(), "trajectory")

    def pscf(self, function):
        self.rightpscf = float(self.ledit_right_pscf.text())
        self.leftpscf = float(self.ledit_left_pscf.text())
        self.toppscf = float(self.ledit_top_pscf.text())
        self.bottompscf = float(self.ledit_bottom_pscf.text())
        self.sizecellpscf = float(self.ledit_cell_size_pscf.text()) 
        if function == "PSCF":
            if self.mode_pscf == "2D_PSCF":
                self.start_threading("2D_PSCF", self.ledit_left_pscf.text(), self.ledit_right_pscf.text(),
                                     self.ledit_bottom_pscf.text(), self.ledit_top_pscf.text(),
                                     self.ledit_cell_size_pscf.text(), self.ledit_criterion_pscf.text())

            elif self.mode_pscf == "3D_PSCF":
                self.start_threading("3D_PSCF", self.ledit_left_pscf.text(), self.ledit_right_pscf.text(),
                                     self.ledit_bottom_pscf.text(), self.ledit_top_pscf.text(),
                                     self.ledit_cell_size_pscf.text(), self.layers.layers_altitude_pscf,
                                     self.ledit_criterion_pscf.text())

        elif function == "WPSCF":
            if self.mode_pscf == "2D_PSCF":
                if self.weight_PSCF == "endpoints":
                    self.start_threading("2D_WPSCF", self.ledit_reduce_ratio_pscf.text(), self.ledit_weight_pscf.text(),
                                         self.ledit_left_pscf.text(), self.ledit_bottom_pscf.text(),
                                         self.ledit_cell_size_pscf.text(), "endpoints")

                elif self.weight_PSCF == "trajectory":
                    self.start_threading("2D_WPSCF", self.ledit_reduce_ratio_pscf.text(), self.ledit_weight_pscf.text(),
                                         self.ledit_left_pscf.text(), self.ledit_bottom_pscf.text(),
                                         self.ledit_cell_size_pscf.text(), "trajectory")

            elif self.mode_pscf == "3D_PSCF":
                if self.weight_PSCF == "endpoints":
                    self.start_threading("3D_WPSCF", self.layers.red_ratio_endp_pscf, self.layers.nbr_endp_pscf,
                                         self.ledit_left_pscf.text(), self.ledit_bottom_pscf.text(),
                                         self.ledit_cell_size_pscf.text(), "endpoints")

                elif self.weight_PSCF == "trajectory":
                    self.start_threading("3D_WPSCF", self.layers.red_ratio_traj_pscf, self.layers.nbr_traj_pscf,
                                         self.ledit_left_pscf.text(), self.ledit_bottom_pscf.text(),
                                         self.ledit_cell_size_pscf.text(), "trajectory")

    def clustering(self, function):
        if function == "kmean":
            _ama.clustering_kmean(int(self.sBox_ncl_initial_kmean.value()), int(self.sBox_nth_kmean.value()),
                                   int(self.sBox_skip_kmean.value()), int(self.sBox_repetition_clustering.value()))
            self.dir_clust_results()
            
        elif function == "TSV":
            _ama.clustering_TSV(int(self.sBox_nth_tsv.value()), int(self.sBox_skip_tsv.value()))

    def figure_size(self,function):
        if function == "kmean":
            self.layers.figure_plot_clustering(self.nbr_clusters_kmean)
        elif function == "TSV":
            self.layers.figure_plot_clustering(self.nbr_clusters_tsv)

    def choice_nbr_clusters(self, function):
        if function == "kmean":
            self.nbr_clusters_kmean = int(self.sBox_ncl_kmean.value())
            _ama.choice_the_number_of_clusters(self.nbr_clusters_kmean,"kmean", self.dir_clustering)
        elif function == "TSV":
            self.nbr_clusters_tsv = int(self.sBox_ncl_tsv.value())
            _ama.choice_the_number_of_clusters(self.nbr_clusters_tsv, "TSV", "")

    def frequency(self,  figure, canvas, function):
        if function == "kmean":
            _ama.clusters_monthly_frequency("kmean", self.dir_clustering)
            self.freq_file = pd.read_table(self.dir_clustering + "/clusters_monthly_frequency_kmean.csv", sep=";")
            nbr_clust = self.nbr_clusters_kmean

        elif function == "TSV":
            _ama.clusters_monthly_frequency("TSV", "")
            self.freq_file = pd.read_table("../Results/Cluster/clusters_monthly_frequency_tsv.csv", sep=";")
            nbr_clust = self.nbr_clusters_tsv
        self.ss = np.sum(self.freq_file.iloc[:,1:], axis = 1)
        for i in range(12):
            for j in range(nbr_clust): 
                self.freq_file.iloc[i,j+1] = self.freq_file.iloc[i,j+1]/self.ss[i]
        self.bottom_bar = [0] * 12
        self.ax = figure.add_subplot()
        for i in range(nbr_clust):
            self.ax.bar(self.freq_file.iloc[:, 0], self.freq_file.iloc[:, i + 1], 1.0, bottom=self.bottom_bar,label=list(self.freq_file)[i + 1], edgecolor='black')
            self.bottom_bar = list(np.add(self.bottom_bar, self.freq_file.iloc[:, i + 1]))
        plt.xlabel("Months")
        plt.ylabel("Clusters monthly frequency")
        plt.xlim([-0.5, 11.5])
        plt.ylim([0, 1])
        plt.legend(ncol = nbr_clust, bbox_to_anchor= (0.5,-0.1), loc='upper center')
        canvas.draw()

    def rose_plot(self, figure, canvas, function):
        if function == "kmean":
            ctr_name = _ama.extract_clusters_roseplot_file("kmean", self.dir_clustering)
            self.rose_file = pd.read_table(self.dir_clustering + "/roseplot_file_"+ ctr_name +"_kmean.csv", sep=";")
            nbr_clust = self.nbr_clusters_kmean
        elif function == "TSV":
            ctr_name = _ama.extract_clusters_roseplot_file("TSV", "")
            self.rose_file = pd.read_table("../Results/Cluster/roseplot_file_"+ ctr_name +"_tsv.csv", sep=";")
            nbr_clust = self.nbr_clusters_tsv

        nbr_groups = self.nbr_groups
        self.bottom_bar = [0] * nbr_clust
        self.ax = figure.add_subplot(projection='polar')
        plt.axis('off')
        self.width = 2*np.pi /nbr_clust
        self.indexes = list(range(1, nbr_clust + 1))
        self.angles = [element * self.width for element in self.indexes]
        
        for i in range(nbr_groups):
            self.bars = self.ax.bar(
            x=self.angles, 
            height=self.rose_file.iloc[:, i + 1], 
            width=self.width,
            label = f'{self.rose_file.columns[i+1]}', 
            bottom=self.bottom_bar,
            edgecolor="black")
            self.bottom_bar = list(np.add(self.bottom_bar, self.rose_file.iloc[:, i + 1]))
            if (i == nbr_groups-1):
                x=max([self.bars[i].get_height()+self.bottom_bar[i] for i in range(len(self.bars))])
                for bar, angle, height, label, bot in zip(self.bars, self.angles, self.rose_file.iloc[:, i + 1], self.rose_file.iloc[:, 0],self.bottom_bar):
                    # Labels are rotated. Rotation must be specified in degrees :(
                    rotation = np.rad2deg(angle)
                    # Flip some labels upside down
                    alignment = ""
                    if angle >= np.pi/2 and angle < 3*np.pi/2:
                        alignment = "right"
                        rotation = rotation + 180
                    else: 
                        alignment = "left"
                    # Finally add the labels
                    self.ax.text(
                        x=angle, 
                        y= bot + bar.get_height()+x*0.03, 
                        s=label, 
                        ha='center', 
                        va='center', 
                        #rotation=rotation, 
                        rotation_mode="anchor") 
            
            
            
            #self.ax.bar(self.rose_file.iloc[:, 0], self.rose_file.iloc[:, i + 1], 1.0, bottom=self.bottom_bar, label=list(self.rose_file)[i + 1], edgecolor='black')
            #self.ax.bar(self.rose_file.iloc[:, 0], self.rose_file.iloc[:, i + 1], 0.6, bottom=self.bottom_bar, label=list(self.rose_file)[i + 1], edgecolor='black')
            #self.bottom_bar = list(np.add(self.bottom_bar, self.rose_file.iloc[:, i + 1]))
        plt.yticks([])
        plt.legend(bbox_to_anchor= (1.05,1), loc='upper left')
        canvas.draw()

    def plot_var_TSV(self, figure, canvas, function):
        if (function =="kmean"):
            _ama.variation_TSV("kmean", self.dir_clustering)  
            self.var_tsv_file = pd.read_table(self.dir_clustering + "/variation_TSV_kmean.csv", sep=";")
        elif (function =="TSV"):
            _ama.variation_TSV("TSV", "")
            self.var_tsv_file = pd.read_table("../Results/Cluster/variation_TSV_tsv.csv", sep=";")
        self.nbr_clst = self.var_tsv_file['n_clusters']
        self.tsv = self.var_tsv_file['tsv']
        self.var_tsv = self.var_tsv_file['var_tsv']
        ax = figure.add_subplot()
        if function == "kmean":
            if self.btn_rd_var_kmean.isChecked():
                ax.plot(self.nbr_clst, self.var_tsv, linewidth=2.0, marker='.')
                ax.set(xlabel='Number of clusters', ylabel='Variation in Total Spatial Variance %')
            elif self.btn_rd_tsv_kmean.isChecked():
                ax.plot(self.nbr_clst, self.tsv, linewidth=2.0, marker='.')
                ax.set(xlabel='Number of clusters', ylabel='Total Spatial Variance')
        elif function == "TSV":
            if self.btn_rd_var_tsv_tsv.isChecked():
                ax.plot(self.nbr_clst, self.var_tsv, linewidth=2.0, marker='.')
                ax.set(xlabel='Number of clusters', ylabel='Variation in Total Spatial Variance %')
            elif self.btn_rd_tsv_tsv.isChecked():
                ax.plot(self.nbr_clst, self.tsv, linewidth=2.0, marker='.')
                ax.set(xlabel='Number of clusters', ylabel='Total Spatial Variance')
        ax.set_xticks(self.nbr_clst)
        canvas.draw()
    

    
    def plot_clustering(self, figure, canvas, function):
        if function == "kmean":
            self.centroides = pd.read_table( self.dir_clustering +'/meanclsters_Kmean.csv', sep=";")
            self.nbr_clust = self.nbr_clusters_kmean
        elif function == "TSV":
            self.centroides = pd.read_table('../Results/Cluster/meanclsters_TSV.csv', sep=";")
            self.nbr_clust = self.nbr_clusters_tsv
        # map
        ax = figure.add_subplot(1, 1, 1, projection=ccrs.PlateCarree())
        #ax.set_extent([self.min_lon, self.max_lon, self.min_lat, self.max_lat], crs=ccrs.PlateCarree())
        ax.set_extent([float(self.layers.figure_size[0]), float(self.layers.figure_size[1]),
                       float(self.layers.figure_size[2]), float(self.layers.figure_size[3])], crs=ccrs.PlateCarree())

        # ax.add_feature(cfeature.LAND)
        # ax.add_feature(cfeature.OCEAN)
        ax.add_feature(cfeature.COASTLINE)
        ax.add_feature(cfeature.BORDERS, linestyle=':')
        ax.gridlines(draw_labels=True, dms=True, x_inline=False, y_inline=False, )
        # ax.add_feature(cfeature.LAKES, alpha=0.5)
        # ax.add_feature(cfeature.RIVERS)
        # = = = = = = = = = = = = = = = = data plot = = = = = = = = = = = = = = = =
        self.clusters_legend =[]
        for i in range(self.nbr_clust):
            #fig = plt.plot(self.centroides.iloc[:, 2 * i + 1], self.centroides.iloc[:, 2 * i], lw=3)
            ax.plot(self.centroides.iloc[1:, 2 * i + 1], self.centroides.iloc[1:, 2 * i], lw=3) # ?????????? 1:
            self.clusters_legend.append(f'{self.layers.clusters_names[i]} {self.centroides.iloc[0, 2 * i + 1]*100:.2f}%')
                                        # {self.centroides.iloc[1:, 2 * i + 1]})
        ax.legend(self.clusters_legend)
        canvas.draw()

    def clustering_kmean(self):
        self.start_threading("clst_kmean", self.sBox_ncl_kmean.value(), self.sBox_ncl_initial_kmean.value(),
                                     self.sBox_nth_kmean.value())

    def start_threading(self, function, para1=None, para2=None, para3=None, para4=None, para5=None, para6=None, para7=None):
        self.thread_started = OutputGrabber(function, para1, para2, para3, para4, para5, para6, para7)
        self.thread_started.start()
        self.thread_started.mysignal.connect(self.append_text)
        self.thread_started.signal_header.connect(self.data_headers)

    def are_you_sure(self, fonction):
        if fonction == "clear_traj":
            m = QtWidgets.QMessageBox.question(self, "Warning", "Are you sure you want to clear the trajectories ?")
            if m == QtWidgets.QMessageBox.Yes:
                self.start_threading("clear_traj")
        elif fonction == "clear_data":
            m = QtWidgets.QMessageBox.question(self, "Warning", "Are you sure you want to clear the data ?")
            if m == QtWidgets.QMessageBox.Yes:
                self.start_threading("clear_data")
                self.cBox_date_begin.clear()
                self.cBox_date_end.clear()
                self.cBox_data.clear()
                self.cBox_data_sel.clear()
                self.cBox_data_correlation.clear()

    def radio(self, mode):
        if mode == "2D_CWT":
            self.mode_cwt = "2D_CWT"
            self.sBox_layers_cwt.setReadOnly(True)
            self.sBox_layers_cwt.setStyleSheet('background-color:#A9B4B2')
            self.ledit_weight_cwt.setReadOnly(False)
            self.ledit_weight_cwt.setStyleSheet('background-color:')
            self.ledit_reduce_ratio_cwt.setReadOnly(False)
            self.ledit_reduce_ratio_cwt.setStyleSheet('background-color:')
            self.btn_create_layers_cwt.hide()
            self.btn_create_weights_cwt.hide()

        elif mode == "3D_CWT":
            self.mode_cwt = "3D_CWT"
            self.sBox_layers_cwt.setReadOnly(False)
            self.sBox_layers_cwt.setStyleSheet('background-color:')
            self.ledit_weight_cwt.setReadOnly(True)
            self.ledit_weight_cwt.setStyleSheet('background-color:#A9B4B2')
            self.ledit_reduce_ratio_cwt.setReadOnly(True)
            self.ledit_reduce_ratio_cwt.setStyleSheet('background-color:#A9B4B2')
            self.btn_create_layers_cwt.show()
            self.btn_create_weights_cwt.show()

        if mode == "2D_PSCF":
            self.mode_pscf = "2D_PSCF"
            self.sBox_layers_pscf.setReadOnly(True)
            self.sBox_layers_pscf.setStyleSheet('background-color:#A9B4B2')
            self.ledit_weight_pscf.setReadOnly(False)
            self.ledit_weight_pscf.setStyleSheet('background-color:')
            self.ledit_reduce_ratio_pscf.setReadOnly(False)
            self.ledit_reduce_ratio_pscf.setStyleSheet('background-color:')
            self.btn_create_layers_pscf.hide()
            self.btn_create_weights_pscf.hide()

        elif mode == "3D_PSCF":
            self.mode_pscf = "3D_PSCF"
            self.sBox_layers_pscf.setReadOnly(False)
            self.sBox_layers_pscf.setStyleSheet('background-color:')
            self.ledit_weight_pscf.setReadOnly(True)
            self.ledit_weight_pscf.setStyleSheet('background-color:#A9B4B2')
            self.ledit_reduce_ratio_pscf.setReadOnly(True)
            self.ledit_reduce_ratio_pscf.setStyleSheet('background-color:#A9B4B2')
            self.btn_create_layers_pscf.show()
            self.btn_create_weights_pscf.show()

        elif mode == "trajectory_CWT":
            self.weight_CWT = "trajectory"
            self.lab_weight_traj_endp_cwt.setText("Trajectories")
            self.btn_create_weights_cwt.clicked.connect(lambda x: self.layers.layers_weight_traj_cwt())
            #self.ledit_weight_cwt.setReadOnly(True)
            #self.ledit_weight_cwt.setStyleSheet('background-color:#A9B4B2')

        elif mode == "endpoints_CWT":
            self.weight_CWT = "endpoints"
            self.lab_weight_traj_endp_cwt.setText("Endpoints")
            self.btn_create_weights_cwt.clicked.connect(lambda x: self.layers.layers_weight_endp_cwt())
            #self.ledit_weight_cwt.setReadOnly(False)
            #self.ledit_weight_cwt.setStyleSheet('background-color:')

        elif mode == "trajectory_PSCF":
            self.weight_PSCF = "trajectory"
            self.lab_weight_traj_endp_pscf.setText("Trajectories")
            self.btn_create_weights_pscf.clicked.connect(lambda x: self.layers.layers_weight_traj_pscf())

        elif mode == "endpoints_PSCF":
            self.weight_PSCF = "endpoints"
            self.lab_weight_traj_endp_pscf.setText("Endpoints")
            self.btn_create_weights_pscf.clicked.connect(lambda x: self.layers.layers_weight_endp_pscf())

        elif mode == "plot_cwt":
            self.cwt_plot = "CWT"

        elif mode == "plot_wcwt":
            self.cwt_plot = "WCWT"

        elif mode == "plot_pscf":
            self.pscf_plot = "PSCF"

        elif mode == "plot_wpscf":
            self.pscf_plot = "WPSCF"

    def data_headers(self,text):
        if (text == "read"):
            self.cBox_date_begin.clear()
            self.cBox_date_end.clear()
            self.cBox_data.clear()
            self.cBox_data_sel.clear()
            self.cBox_data_correlation.clear()
            if os.path.isfile("../Working/headers.txt"):
                with open("../Working/headers.txt") as file:
                    for line in file:
                        self.cBox_date_begin.addItem(line.rstrip())
                        self.cBox_date_end.addItem(line.rstrip())
                        self.cBox_data.addItem(line.rstrip())
                        self.cBox_data_sel.addItem(line.rstrip())
                        self.cBox_data_correlation.addItem(line.rstrip())
        if (text == "clear"):
            self.cBox_date_begin.clear()
            self.cBox_date_end.clear()
            self.cBox_data.clear()
            self.cBox_data_sel.clear()
            self.cBox_data_correlation.clear()
    


    def append_text(self, text):
        self.output.moveCursor(QtGui.QTextCursor.End)
        self.output.insertPlainText(text)

    def plot_initial(self, figure, canvas):
        ax = figure.add_subplot(1, 1, 1, projection=ccrs.PlateCarree())
        ax.set_extent([-58, 40, -5, 65], crs=ccrs.PlateCarree())
        ax.add_feature(cfeature.LAND)
        ax.add_feature(cfeature.OCEAN)
        ax.add_feature(cfeature.COASTLINE)
        ax.add_feature(cfeature.BORDERS, linestyle=':')
        #ax.gridlines(draw_labels=True, dms=True, x_inline=False, y_inline=False, )
        ax.add_feature(cfeature.LAKES, alpha=0.5)
        ax.add_feature(cfeature.RIVERS)
        canvas.draw()
        
    def plot_cwt(self, i, function, figure, canvas):
        if function == "CWT":
            if self.mode_cwt == "3D_CWT":
                self.titel_cwt = f'CWT {i}'
                self.cwt_file = pd.read_table(f'../Results/CWT/cwt{i}.csv', sep=";")
                self.y_list = self.cwt_file['lon']
                self.x_list = self.cwt_file['lat']
                self.z_list = self.cwt_file['cwt']
            elif self.mode_cwt == "2D_CWT":
                self.titel_cwt = 'CWT'
                self.cwt_file = pd.read_table('../Results/CWT/cwt.csv', sep=";")
                self.y_list = self.cwt_file['lon']
                self.x_list = self.cwt_file['lat']
                self.z_list = self.cwt_file['cwt']
        elif function == "WCWT":
            if self.mode_cwt == "3D_CWT":
                self.titel_cwt = f'WCWT {i}'
                self.cwt_file = pd.read_table(f'../Results/CWT/wcwt{i}.csv', sep=";")
                self.y_list = self.cwt_file['lon']
                self.x_list = self.cwt_file['lat']
                self.z_list = self.cwt_file['wcwt']
            elif self.mode_cwt == "2D_CWT":
                self.titel_cwt = 'WCWT'
                self.cwt_file = pd.read_table('../Results/CWT/wcwt.csv', sep=";")
                self.y_list = self.cwt_file['lon']
                self.x_list = self.cwt_file['lat']
                self.z_list = self.cwt_file['wcwt']
        self.mx = max(self.z_list)
        self.mn = min(self.z_list)
        self.r = self.mx - self.mn
        self.z = self.z_list.to_numpy()
        self.heatmap = np.reshape(self.z,(int((self.topcwt - self.bottomcwt)/self.sizecellcwt) ,int((self.rightcwt-self.leftcwt)/self.sizecellcwt)))
        # = = = = = = = = = = = = = =  map = = = = = = = = = =
        ax = figure.add_subplot(1, 1, 1, projection=ccrs.PlateCarree())
        ax.set_extent([min(self.x_list), max(self.x_list), min(self.y_list), max(self.y_list)], crs=ccrs.PlateCarree())
        # ax.add_feature(cfeature.LAND)
        # ax.add_feature(cfeature.OCEAN)
        ax.add_feature(cfeature.COASTLINE)
        ax.add_feature(cfeature.BORDERS, linestyle=':')
        ax.gridlines(draw_labels=True, dms=True, x_inline=False, y_inline=False, )
        # ax.add_feature(cfeature.LAKES, alpha=0.5)
        # ax.add_feature(cfeature.RIVERS)
        # ======================================= data plot
        # =======================================
        self.heatmap[self.heatmap == 0] = np.nan
        cmap = plt.get_cmap(self.cBox_colorbar_cwt.currentText(), self.sBox_bounds_cwt.value())
        
        figure = plt.imshow(self.heatmap, extent=[min(self.x_list), max(self.x_list), min(self.y_list), max(self.y_list)],
                            interpolation='none', origin="lower", cmap=cmap, vmin =0)
        plt.colorbar(figure, cmap=cmap, fraction=0.030, pad=0.1)
        plt.title(self.titel_cwt)
        canvas.draw()

    def plot_pcsf(self, i, function, figure, canvas):
        if function == "PSCF":
            if self.mode_pscf == "3D_PSCF":
                self.titel_pscf =f'PSCF {i}'
                self.pscf_file = pd.read_table(f'../Results/PSCF/pscf{i}.csv', sep=";")
                self.y_list = self.pscf_file['lon']
                self.x_list = self.pscf_file['lat']
                self.z_list = self.pscf_file['pscf']
            elif self.mode_pscf == "2D_PSCF":
                self.titel_pscf = 'PSCF'
                self.pscf_file = pd.read_table('../Results/PSCF/pscf.csv', sep=";")
                self.y_list = self.pscf_file['lon']
                self.x_list = self.pscf_file['lat']
                self.z_list = self.pscf_file['pscf']
        elif(function == "WPSCF"):
            if self.mode_pscf == "3D_PSCF":
                self.titel_pscf = f'WPSCF {i} '
                self.pscf_file = pd.read_table(f'../Results/PSCF/wpscf{i}.csv', sep=";")
                self.y_list = self.pscf_file['lon']
                self.x_list = self.pscf_file['lat']
                self.z_list = self.pscf_file['wpscf']
            elif self.mode_pscf == "2D_PSCF":
                self.titel_pscf = 'WPSCF'
                self.pscf_file = pd.read_table('../Results/PSCF/wpscf.csv', sep=";")
                self.y_list = self.pscf_file['lon']
                self.x_list = self.pscf_file['lat']
                self.z_list = self.pscf_file['wpscf']

        # = = = = = = = = = = = = = =  map = = = = = = = = = =
        self.mx = max(self.z_list)
        self.mn = min(self.z_list)
        self.r = self.mx - self.mn
        self.z = self.z_list.to_numpy()
        self.heatmap = np.reshape(self.z,(int((self.toppscf - self.bottompscf)/self.sizecellpscf) ,int((self.rightpscf-self.leftpscf)/self.sizecellpscf)))
        # = = = = = = = = = = = = = =  map = = = = = = = = = =
        ax = figure.add_subplot(1, 1, 1, projection=ccrs.PlateCarree())
        ax.set_extent([min(self.x_list), max(self.x_list), min(self.y_list), max(self.y_list)], crs=ccrs.PlateCarree())
        # ax.add_feature(cfeature.LAND)
        # ax.add_feature(cfeature.OCEAN)
        ax.add_feature(cfeature.COASTLINE)
        ax.add_feature(cfeature.BORDERS, linestyle=':')
        ax.gridlines(draw_labels=True, dms=True, x_inline=False, y_inline=False, )
        # ax.add_feature(cfeature.LAKES, alpha=0.5)
        # ax.add_feature(cfeature.RIVERS)
        # ======================================= data plot
        # =======================================
        self.heatmap[self.heatmap == 0] = np.nan
        #cmap = plt.get_cmap('jet', 20)
        cmap = plt.get_cmap(self.cBox_colorbar_pscf.currentText(), self.sBox_bounds_pscf.value())
        figure = plt.imshow(self.heatmap, extent=[min(self.x_list), max(self.x_list), min(self.y_list), max(self.y_list)],
                            interpolation='none', origin="lower", cmap=cmap, vmin =0)
        plt.colorbar(figure, cmap=cmap, fraction=0.030, pad=0.1)
        plt.title(self.titel_pscf)
        canvas.draw()

app = QtWidgets.QApplication(sys.argv)
window = Mainwindow()
app.exec_()
