%module ama
%include "std_string.i"
%{
	#include <iostream>
	#include <algorithm>
	#include <vector>
	#include <dirent.h>
	#include <string>
	#include <string.h>
	#include <fstream>
	#include <sstream>
	#include <cmath>
	#include <iomanip>
	#include <thread>
	#include <chrono>
	#include <cstdlib>
	#include <ctime>
	#include <filesystem>
	#include <sys/stat.h>

	extern int n_traj, e, nr,nc,nl, nh;
	/************  data info **********/
	extern int index_date_begin, index_date_end ,index_concentration , index_groups , nbr_clusters;
	extern bool clustersadded, groupsadded , date_begin_converted, date_end_converted;
	extern std::vector<std::string> clusters_names,group_names;
	extern std::vector<double>group_values;
	extern double missing_values;
	/**********************************/
	struct Meteorological_Parameters{
	    std::string header;
	    std::vector<double> column;
	};
	struct Traj{
		int id,traj_nbr,day,month,year,len_tra=0,cluster=-2,date_num;
		std::string date, cluster_name;
		std::vector <std::string> date2;
		std::vector <double> hour_in,lat,lon,alt;
		std::vector<Meteorological_Parameters> meteo_para;
		double ctr=0;
		int i=0;                                // it's not used for now !!!!
	};

	struct datta{
		std::string header;
		std::vector<std::string> column;
	};

	struct cell{
		double sumC=0.0,cwt=0.0,pscf=0.0, wcwt=0.0,wpscf=0.0,r=0,sumP=0,moy_c=0,moy_p=0,sd_p=0,sd_c=0; // r correlation
		int duration=0,n=0;
		std::vector<int> tr;			//trajectories in cell
		std::vector<double> ctr,parameter; // correlation
	};

	struct cluster{
		std::vector<int> trajectories;
		std::vector<double> lat_moy,lon_moy;
		long double csv=0.0;
	};

	struct TS{
		int c1=n_traj+2,c2=n_traj+2;
		long double csv=0,tsv=0;
		std::vector<double> la , lo;
	};

	extern std::vector<std::string> listfiles;	/*************list files*************/
	extern std::vector<TS> TV_th;
	extern std::vector<std::vector<cluster>> clusters;
	extern std::vector<Traj> trajectory;
	extern std::vector<datta> data;
	// extern std::vector<datta> data_temp;
	extern std::vector<std::vector<std::vector<cell>>> celle;
	extern std::vector<long double> var_tsv;
	
	extern bool check_dir_exist(const char* folder);
	
	extern void delete_dir_content(const std::filesystem::path& dir_path);
	
	extern void create_dir(const char* folder);
	
	extern void clear_clusters();

	extern void clear_trajs();

	extern void clear_data();

	extern void display_data();
	
	extern int get_index_data_headers(std::string header, std::string Type);
	
	extern void remove_columns_data(std::string datatoremove);
	
	extern void fill_clusters_names(std::string header_clusters);
	
	extern std::string fill_group_names(std::string groups);

	extern int date_to_num(std::string date);
	
	extern void read_listfile();

	extern void read_trajectories(char path[500]);

	extern void printallTrajectories();
	
	extern void printTrajectory(int i);

	extern void read_data(std::string path_data, std::string header);
	
	extern void datamonthly_average(int indx);
	
	extern void date_format(std::string date_column, std::string date_format);

	extern void select_trajs_ctr(double min, double max, std::string path_dir);

	extern void concentration_trajectories(std::string eend);

	extern void ctr_trajs(double missing_value);

	extern void correlation(double left, double right, double bottom, double top, double sz, std::string para);

	extern void CWT(double left, double right, double bottom, double top, double sz);

	extern void WCWT(std::string reduce_ratio, std::string weights, double left, double bottom, double sz, std::string weight);

	extern void CWT_3D(double left, double right, double bottom, double top, double sz, std::string heights);

	extern void WCWT_3D(std::string reduce_ratios, std::string weightss, double left, double bottom, double sz, std::string weight);

	extern void PSCF(double left, double right, double bottom, double top, double sz, double criterion);

	extern void WPSCF(std::string reduce_ratio, std::string weights, double left, double bottom, double sz, std::string weight);

	extern void PSCF_3D(double left, double right, double bottom, double top, double sz, std::string heights,double criterion);

	extern void WPSCF_3D(std::string reduce_ratios, std::string weightss, double left, double bottom, double sz, std::string weight);

	extern void fun_TSV (std::vector<cluster> clust,int i,int s,int num_thread,long double tsv,int nh, int skip);

	extern void clustering_TSV (int nth, int skip);

	extern void clustering_kmean (int ncl_initial, int nth, int skip, int repetition);
	
	extern void variation_TSV(std::string funct, std::string path_dir);
	
	extern void choice_the_number_of_clusters (int ncl, std::string funct, std::string path_dir);

	extern void data_to_trajstat ();

	extern void extract_file_data(std::string path,  double min, double max, std::string header,std::string file);
	
	extern void add_clusters_to_data_file (std::string funct, std::string path_dir);
	
	extern std::string add_groups_to_data_file (std::string funct, std::string path_dir);
	
	extern std::string extract_clusters_roseplot_file(std::string funct, std::string path_dir);
	
	extern void clusters_monthly_frequency(std::string funct, std::string path_dir);
	
	extern void open_clustering(std::string func, std::string path_dir);
	
	extern void freq_hysplit(int ncl,std::string path);

%}
	
	extern int n_traj, e, nr,nc,nl, nh;
	/************  data info **********/
	extern int index_date_begin, index_date_end ,index_concentration , index_groups , nbr_clusters;
	extern bool clustersadded, groupsadded , date_begin_converted, date_end_converted;
	extern std::vector<std::string> clusters_names,group_names;
	extern std::vector<double>group_values;
	extern double missing_values;
	/**********************************/
	struct Meteorological_Parameters{
	    std::string header;
	    std::vector<double> column;
	};
	struct Traj{
		int id,traj_nbr,day,month,year,len_tra=0,cluster=-2,date_num;
		std::string date, cluster_name;
		std::vector <std::string> date2;
		std::vector <double> hour_in,lat,lon,alt;
		std::vector<Meteorological_Parameters> meteo_para;
		double ctr=0;
		int i=0;                                // it's not used for now !!!!
	};

	struct datta{
		std::string header;
		std::vector<std::string> column;
	};

	struct cell{
		double sumC=0.0,cwt=0.0,pscf=0.0, wcwt=0.0,wpscf=0.0,r=0,sumP=0,moy_c=0,moy_p=0,sd_p=0,sd_c=0; // r correlation
		int duration=0,n=0;
		std::vector<int> tr;			//trajectories in cell
		std::vector<double> ctr,parameter; // correlation
	};

	struct cluster{
		std::vector<int> trajectories;
		std::vector<double> lat_moy,lon_moy;
		long double csv=0.0;
	};

	struct TS{
		int c1=n_traj+2,c2=n_traj+2;
		long double csv=0,tsv=0;
		std::vector<double> la , lo;
	};

	extern std::vector<std::string> listfiles;	/*************list files*************/
	extern std::vector<TS> TV_th;
	extern std::vector<std::vector<cluster>> clusters;
	extern std::vector<Traj> trajectory;
	extern std::vector<datta> data;
	//extern std::vector<datta> data_temp;
	extern std::vector<std::vector<std::vector<cell>>> celle;
	extern std::vector<long double> var_tsv;
	
	extern bool check_dir_exist(const char* folder);
	
	extern void delete_dir_content(const std::filesystem::path& dir_path);
	
	extern void create_dir(const char* folder);

	extern void clear_clusters();

	extern void clear_trajs();

	extern void clear_data();

	extern void display_data();
	
	extern int get_index_data_headers(std::string header, std::string Type);
	
	extern void remove_columns_data(std::string datatoremove);
	
	extern void fill_clusters_names(std::string header_clusters);
	
	extern std::string fill_group_names(std::string groups);

	extern int date_to_num(std::string date);
	
	extern void read_listfile();

	extern void read_trajectories(char path[500]);

	extern void printallTrajectories();
	
	extern void printTrajectory(int i);

	extern void read_data(std::string path_data, std::string header);
	
	extern void datamonthly_average(int indx);
	
	extern void date_format(std::string date_column, std::string date_format);

	extern void select_trajs_ctr(double min, double max, std::string path_dir);

	extern void concentration_trajectories(std::string eend);

	extern void ctr_trajs(double missing_value);

	extern void correlation(double left, double right, double bottom, double top, double sz,std::string para);

	extern void CWT(double left, double right, double bottom, double top, double sz);

	extern void WCWT(std::string reduce_ratio, std::string weights, double left, double bottom, double sz, std::string weight);

	extern void CWT_3D(double left, double right, double bottom, double top, double sz, std::string heights);

	extern void WCWT_3D(std::string reduce_ratios, std::string weightss, double left, double bottom, double sz, std::string weight);

	extern void PSCF(double left, double right, double bottom, double top, double sz, double criterion);

	extern void WPSCF(std::string reduce_ratio, std::string weights, double left, double bottom, double sz, std::string weight);

	extern void PSCF_3D(double left, double right, double bottom, double top, double sz, std::string heights,double criterion);

	extern void WPSCF_3D(std::string reduce_ratios, std::string weightss, double left, double bottom, double sz, std::string weight);

	extern void fun_TSV (std::vector<cluster> clust,int i,int s,int num_thread,long double tsv,int nh, int skip);

	extern void clustering_TSV (int nth, int skip);

	extern void clustering_kmean (int ncl_initial, int nth, int skip, int repetition);
	
	extern void variation_TSV(std::string funct, std::string path_dir);
	
	extern void choice_the_number_of_clusters (int ncl, std::string funct, std::string path_dir);

	extern void data_to_trajstat ();

	extern void extract_file_data(std::string path,  double min, double max, std::string header,std::string file);
	
	extern void add_clusters_to_data_file (std::string funct, std::string path_dir);
	
	extern std::string add_groups_to_data_file (std::string funct, std::string path_dir);
	
	extern std::string extract_clusters_roseplot_file(std::string funct, std::string path_dir);
	
	extern void clusters_monthly_frequency(std::string funct, std::string path_dir);
	
	extern void open_clustering(std::string func, std::string path_dir);

	extern void freq_hysplit(int ncl,std::string path);

