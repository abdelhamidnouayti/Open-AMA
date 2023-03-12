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

int n_traj = 0, e=0, nr=0,nc=0,nl=0, nh =0;     // nr=nbr rows; nc =nbr colomns ;nl = nbr levels;
/************  data info **********/
int index_date_begin = -1,index_date_end =-1,index_concentration =-1, index_groups = -1, nbr_clusters = 0;
bool clustersadded = false, groupsadded = false, date_begin_converted =false, date_end_converted = false;
std::vector<std::string> clusters_names,group_names;
std::vector<double>group_values;
double missing_values;
/**********************************/
struct Meteorological_Parameters{
    std::string header;
    std::vector<double> column;
};

struct Traj{
    int id,traj_nbr,hour,day,month,year,len_tra=0,cluster=-2,date_num;
    std::string date, cluster_name;
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
    std::vector<int> tr;			//tr : trajectories in cell
    std::vector<double> ctr,parameter; // parameter : correlation
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

std::vector<std::string> listfiles;	/************* list files*************/
std::vector<TS> TV_th;
std::vector<std::vector<cluster>> clusters;
std::vector<Traj> trajectory;
std::vector<datta> data;
std::vector<datta> data_clusters;
datta data_groups_rose;

// std::vector<datta> data_temp;
std::vector<std::vector<std::vector<cell>>> celle;
std::vector<long double> var_tsv;



std::vector<std::vector<double>> matrix_addition(std::vector<std::vector<double>> a,std::vector<std::vector<double>> b) {
    int ROWS=0, COLS=2;

    std::vector<std::vector<double>> c(ROWS, std::vector<double>(COLS, 0));

    // matrix addition
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }

    // display the result
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            std::cout << c[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return c;
}

std::vector<std::vector<double>> matrix_multiplication(std::vector<std::vector<double>> a, std::vector<std::vector<double>> b) {
    int ROWS = 2,COLS=3;
    std::vector<std::vector<double>> c(ROWS, std::vector<double>(ROWS, 0));
    
    // matrix multiplication
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < ROWS; ++j) {
            for (int k = 0; k < COLS; ++k) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    // display the result
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < ROWS; ++j) {
            std::cout << c[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return c;
}

int transpose_matrix(std::vector<std::vector<int>> a) {
    int ROWS = 2,COLS=3;
    std::vector<std::vector<int>> t(COLS, std::vector<int>(ROWS));

    // transpose the matrix
    for (int i = 0; i < COLS; ++i) {
        for (int j = 0; j < ROWS; ++j) {
            t[i][j] = a[j][i];
        }
    }

    // display the result
    std::cout << "Original matrix:" << std::endl;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            std::cout << a[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "Transposed matrix:" << std::endl;
    for (int i = 0; i < COLS; ++i) {
        for (int j = 0; j < ROWS; ++j) {
            std::cout << t[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}



bool check_dir_exist(const char* folder){
    struct stat sb;
    if (stat(folder, &sb) == 0){
        return true;
    }else{
        return false;
    }
}

void delete_dir_content(const std::filesystem::path& dir_path) {
    for (auto& path: std::filesystem::directory_iterator(dir_path)) {
        std::filesystem::remove_all(path);
    }
}

void create_dir(const char* folder){
    std::filesystem::create_directory(folder);
}

void data_for_trajstat(){
    /*This function is developped in order to create data file for TrajStat*/
    if (trajectory.size()==0){
        std::cout<<"Warning : You must read the trajectory files first.";
        return;
    }else if (data.size() ==0){
        std::cout<<"Warning : You must read the Data file first.";
        return;
    }
    std::ofstream out;
    out.open("../Results/data_for_trajstat.csv");
    out <<"date"<<";"<<"concentration"<<std::endl;
    for(int i =0; i<n_traj;i++){
        out << std::setw(4) << std::setfill('0') << trajectory[i].year;
        out << std::setw(2) << std::setfill('0') << trajectory[i].month;
        out << std::setw(2) << std::setfill('0') << trajectory[i].day;
        out << std::setw(2) << std::setfill('0') << trajectory[i].hour;
        out<<";";
        out << trajectory[i].ctr<<std::endl;
    }
}

void clear_clusters(){
    /*This function is developped in order to clear the clusters*/
    std::cout<<"clear_clusters started"<<std::endl;
    try
    {
        if (clusters.size()!=0){
            clusters.resize(0);
            clusters.shrink_to_fit();
            var_tsv.resize(0);
            var_tsv.shrink_to_fit();
            nbr_clusters = 0;
            clusters_names.resize(0);
            clusters_names.shrink_to_fit();
            clustersadded = false;
            groupsadded = false;
            for (int i = 0; i < n_traj; i++)
            {
                trajectory[i].cluster = -2;
            }
            std::cout<<"done."<<std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    std::cout<<"Clusters have cleared successfully"<<std::endl;
}

void clear_trajs(){
    /*This function is developped in order to clear the Trajectories*/
    std::cout<<"Clear_trajectories started"<<std::endl;
    try
    {
        if (trajectory.size()!=0){
            trajectory.resize(0);
            trajectory.shrink_to_fit();
            listfiles.resize(0);
            listfiles.shrink_to_fit();
            n_traj = trajectory.size();
            std::cout<<"The trajectories have cleared successfully successfully"<<std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
}

void clear_data(){
    /*This function is developped in order to clear data*/
    try
    {
        if (data.size()!=0){
            data.resize(0);
            data.shrink_to_fit();
            clustersadded=false;
            groupsadded = false;
            date_begin_converted = false;
            date_end_converted = false;
            index_concentration =-1;
            index_date_begin = -1;
            index_date_end = -1;
            index_groups = -1;
            std::cout<<"The Data has been cleared successfully."<<std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
}

void display_data(){
	std::cout<<"Started display_data ..."<<std::endl;
    try
    {
        if(data.size()==0){
            std::cout<<"Sorry,you didn't put any data!"<<std::endl;
        }else{
            for(int j=0;j<data.size();j++) {
                    std::cout<<data[j].header<<" ";
            }
            std::cout<<std::endl;
            for (int i=0;i<data[0].column.size();i++) {
                for(int j=0;j<data.size();j++) {
                    std::cout<<data[j].column[i]<<"  ";
                }
                std::cout<<std::endl;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    std::cout<<"Complete display_data ..."<<std::endl;
}

int get_index_data_headers(std::string header, std::string Type){
    int ind =-1;
    
    try
    {
        for(int i=0;i<data.size();i++){
            if(header==data[i].header){
                if (Type=="date_begin"){
                    index_date_begin=i;
                }else if(Type=="date_end"){
                    index_date_end=i;
                }else if(Type=="concentration"){
                    index_concentration=i;
                }else{
                    ind = i;
                    return ind;
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return 0;
    }
    return ind;
}

void remove_columns_data(std::string datatoremove){
    if(datatoremove == "clusters"){
        data_clusters.resize(0);
        data_clusters.shrink_to_fit();
        clusters_names.resize(0);
        clusters_names.shrink_to_fit();
        clustersadded = false;
    }else if(datatoremove == "Groups"){
        data_groups_rose.column.resize(0);
        data_groups_rose.column.shrink_to_fit();
        index_groups = -1;
        groupsadded = false;
        group_names.resize(0);
        group_names.shrink_to_fit();
        group_values.resize(0);
        group_values.shrink_to_fit();
    }
}

void fill_clusters_names(std::string header_clusters){
    std::stringstream ss1(header_clusters);
    std::string str;
    clusters_names.resize(0);
    clusters_names.shrink_to_fit();
    while (std::getline(ss1, str, ';'))
    {
        clusters_names.push_back(str);
    }
}

std::string fill_group_names(std::string groups){
    std::string groups_str = "";
    group_names.resize(0);
    group_names.shrink_to_fit();
    group_values.resize(0);
    group_values.shrink_to_fit();
    std::stringstream ss1(groups);
    std::string str;
    while (std::getline(ss1, str, ';'))
    {
        group_values.push_back(stod(str));
    }
    for(int i =0; i<group_values.size()+1; i++){
        if (i ==0)
        {
            std::stringstream ss;
            std::string st;
            ss << group_values[i];
            ss >> st;
            group_names.push_back("C<"+st);
            continue;
        }else if (i == group_values.size())
        {
            std::stringstream ss;
            std::string st;
            ss << group_values[i-1];
            ss >> st;
            group_names.push_back("C>"+st);
            continue;
        }
        std::stringstream ss1,ss2;
        std::string st1,st2;
        ss1 << group_values[i-1];
        ss2 << group_values[i];
        ss1 >> st1;
        ss2 >> st2;
        group_names.push_back(st1+"<C<"+st2);
    }
    for (int i = 0; i < group_names.size(); i++)
    {
        if (i ==0){
            groups_str = group_names[i];
        }else{
            groups_str = groups_str +';'+ group_names[i];
        }
    }
    
    return groups_str;
}

int date_to_num(std::string date){
    try
    {
        std::stringstream ss(date);
        std::string D,M,Y;
        int day,year,month,days=1,days_in_months;
        std::getline(ss,Y,'-');
        std::getline(ss,M,'-');
        std::getline(ss,D,' ');
        day=stoi(D);
        month = stoi(M);
        year =stoi(Y);
        days= (year-year%4)*366/4+(year-year%4)*365*3/4+year%4*365;
        if(month==1){
            days_in_months=0;
        }else if(month==2){
            days_in_months=31;
        }else if(month==3){
            days_in_months=59;
        }else if(month==4){
            days_in_months=90;
        }else if(month==5){
            days_in_months=120;
        }else if(month==6){
            days_in_months=151;
        }else if(month==7){
            days_in_months=181;
        }else if(month==8){
            days_in_months=212;
        }else if(month==9){
            days_in_months=243;
        }else if(month==10){
            days_in_months=273;
        }else if(month==11){
            days_in_months=304;
        }else if(month==12){
            days_in_months=334;
        }
        if ((year%4)==0 && month>=3){days_in_months=days_in_months;}
            days = days + day + days_in_months;
        return days;
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return 0;
    }
    

}

void read_listfile(){
    std::cout<<"Started read listfiles  ..."<<std::endl;
    trajectory.resize(0);
    trajectory.shrink_to_fit();
    n_traj = 0;
    nh = 0;
    sort(listfiles.begin(), listfiles.end());     //sort list files 
    std::string line;
    std::string file;
    std::ifstream inFile;
    n_traj = listfiles.size();
    trajectory.resize(n_traj);
    int i=0;
    for(auto path:listfiles){
        inFile.open(path);
        int j=0;
        int l=0;
        int r =0;
        std::string y="",m="",d="",h="";
        while (!inFile.eof())
        {
            if (r==0){
                std::getline(inFile, line);
                std::stringstream ss(line);
                std::string n;
                ss>> n;
                for (int x=0;x<stoi(n);x++){
                    std::getline(inFile, line);
                    //std::stringstream ss(line);
                }
                r = 1;
            }
            if (r==1){
                std::getline(inFile, line);
                std::stringstream ss(line);
                std::string n;
                ss>> n;
                
                for (int x=0; x<stoi(n); x++){
                    std::getline(inFile, line);
                }
                r = 2;
            }
            
            if (r==2){
                std::getline(inFile, line);
                std::stringstream ss(line);
                std::string n;
                ss>> n;
                trajectory[i].meteo_para.resize(stoi(n));
                for (int x=0; x<stoi(n); x++){
                    ss>>trajectory[i].meteo_para[x].header;
                }
                r = 3;
            }
            while (std::getline(inFile, line))
            {
                std::stringstream ss(line);
                std::string b1="", b2="", b3="", b4="", b5="", b6="",b7="",b8="",b9="",b10="",b11="",b12="";
                ss>> b1 >> b2 >> b3 >> b4>> b5>> b6 >> b7 >> b8 >> b9 >> b10 >> b11 >> b12;
                for(int x=0;x<trajectory[i].meteo_para.size();x++){
                    std::string n;
                    ss>>n;
                    trajectory[i].meteo_para[x].column.push_back(stod(n));
                }
                if (l==0){
                    y=b3;
                    m=b4;
                    d=b5;
                    h=b6;
                    std::stringstream sss;
                    std::string str;
                    if(stoi(y)<40){
                        sss << stoi(y)+2000;
                        sss >> str;
                        trajectory[i].date=str+"-"+m+"-"+d+" "+h+":"+"00";
                    }
                    else{
                        sss << stoi(y)+1900;
                        sss >> str;
                        trajectory[i].date=str+"-"+m+"-"+d+" "+h+":"+"00";
                    }
                    trajectory[i].day=stoi(d);
                    trajectory[i].month=stoi(m);
                    trajectory[i].year=stoi(str);
                    trajectory[i].hour = stoi(h);
                    trajectory[i].date_num=date_to_num(trajectory[i].date);
                }
                // std::cout<<b1<<" "<< b2<<" "<< b3<<" "<< b4<<" "<< b5<<" "<< b6<<" "<<b7<<" "<<b8<<" "<<b9<<" "<<b10<<" "<<b11<<" "<<b12<<" "<<" "<<std::endl;
                trajectory[i].id=i;
                trajectory[i].traj_nbr = stoi(b1);
                trajectory[i].hour_in.push_back(stod(b9));
                trajectory[i].lat.push_back(stod(b10));
                trajectory[i].lon.push_back(stod(b11));
                trajectory[i].alt.push_back(stod(b12));
                j++;
                l++;
            }
        }
        trajectory[i].len_tra = trajectory[i].lat.size();
        inFile.close();
        i++;
    }
    int t = 0;
    int le = trajectory[0].len_tra;
    for (int i = 0; i<n_traj; i++){
        if (trajectory[i].len_tra != le){
            std::cout<<"The trajectories do not have the same number of endpoints."<<std::endl;
            t=1;
            break;
        }
    }
    if (t==0){
        nh = le;
        std::cout<<"The total number of trajectories is : "<< n_traj <<std::endl;
        std::cout<<"The length of trajectories is : "<< nh <<" endpoints."<<std::endl;
    }else{
        std::cout<<"** Tajectories and their length **"<<std::endl;
        for (int i = 0; i<n_traj; i++){
            std::cout<<listfiles[i]<<" | "<<trajectory[i].len_tra<<std::endl;    
        }
    }
    std::cout<<"Complete read_listfile ..."<<std::endl;
}

void read_trajectories(char path[500]){//,std::vector<Traj> trajectory){
    std::cout<<"Started read_trajectories ..."<<std::endl;
    /*trajectory.resize(0);
    trajectory.shrink_to_fit();
    n_traj = 0;*/
    try
    {
        nh = 0;
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (path)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                if(strcmp(ent->d_name,".")==0 || strcmp( ent->d_name,"..")==0){
                    continue;
                }
                std::string pth=std::string(path)+"/"+std::string(ent->d_name);
                listfiles.push_back(pth);
            }
            closedir (dir);
        } else {
            /* could not open directory */
            std::cout<<"The directory could not be opened";
            return;
        }
        read_listfile();
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        trajectory.resize(0);
        trajectory.shrink_to_fit();
        return;
    }
    std::cout<<"Complete read_trajectories ..."<<std::endl;
    return;
}

void printallTrajectories(){//  std::vector<Traj> trajectory){
    std::cout << "id" <<"   "<< "Date"<<"   "<< "Hour_in"<<"   "<< "Lat"<<"   "<< "Lon"<<"   "<< "Alt"<<"   "<<"Concentration"<<"   "<<"C"<<std::endl;
	try
    {
        for (int i=0; i<n_traj;i++){	
            /*for (int j = 0; j < trajectory[i].lat.size(); j++){
                std::cout << trajectory[i].id <<"   "<< trajectory[i].date<<"   "<< trajectory[i].hour_in[j]<<"   "<< trajectory[i].lat[j]<<"   "<< trajectory[i].lon[j]<<"   "<< trajectory[i].alt[j]<<"   "<<trajectory[i].ctr<<"   "<<trajectory[i].C<<std::endl;
                
            }*/
            std::cout<< trajectory[i].id <<"    "<<trajectory[i].year <<"    "<<trajectory[i].month <<"    "<<trajectory[i].day <<"    "<<trajectory[i].date<<"   "<<"   "<<trajectory[i].date_num<<"   "<<trajectory[i].ctr<<std::endl;
	    }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    
    
}

void printTrajectory(int i){
    try
    {
        if(trajectory.size()==0){
            std::cout<<"You don't read the trajectories yet."<<std::endl;
        }else if(i<0){
            std::cout<<"Sorry! You can't insert a negative number "<<std::endl;
            std::cout<<"You have to put a number were from 0 to "<<n_traj-1<<std::endl;
        }else if (i >= trajectory.size()){
            std::cout<< " Sorry! you insert just "<<n_traj<<" trajectories"<<std::endl;
            std::cout<<"You have to insert a number were from 0 to "<<n_traj-1<<std::endl;	
        }else{
            std::cout << "id" <<"   "<<"traj_nbr"<<"   "<< "Date"<<"   "<< "Hour_in"<<"   "<< "Lat"<<"   "<< "Lon"<<"   "<< "Alt"<<"   "<<"Concentration"<<"   ";
            for(int x=0;x<trajectory[i].meteo_para.size();x++){
                std::cout<<trajectory[i].meteo_para[x].header<<"   ";
            }
            std::cout<<"C"<<std::endl;
            for (int j = 0; j < trajectory[i].lat.size(); j++){
                        std::cout << trajectory[i].id <<"   "<<trajectory[i].traj_nbr<<"   "<< trajectory[i].date<<"   "<< trajectory[i].hour_in[j]<<"   "<< trajectory[i].lat[j]<<"   "<< trajectory[i].lon[j]<<"   "<< trajectory[i].alt[j]<<"   "<<trajectory[i].ctr<<"   ";
                        for(int x=0;x<trajectory[i].meteo_para.size();x++){
                            std::cout<<trajectory[i].meteo_para[x].column[j]<<"   ";
                        }
                        std::cout<<trajectory[i].cluster<<std::endl;
                        
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    
 
}

void read_data(std::string path_data, std::string header){
    std::cout<<"Started read_data ..."<<std::endl;
    try
    {
        std::ifstream datafile;
        std::vector<std::string> headers;
        datafile.open(path_data);
        std::string line;
        int nll = 0, nbr_column=0;
        if (datafile.is_open())
        {
            int j=0;
            while (!datafile.eof())
            {
                if (header == "yes"){			// if the data file has headers == yes
                    std::ofstream out;
                    out.open("../Working/headers.txt");
                    std::getline(datafile, line);
                    std::stringstream ss(line);
                    std::string str;
                    while (std::getline(ss,str,';'))
                    {
                        out<<str<<std::endl;
                        headers.push_back(str);
                    }
                    nbr_column=headers.size();
                    header = "no";
                }
                data.resize(nbr_column);
                while (std::getline(datafile, line))
                {
                    std::stringstream ss(line);
                    for (int i = 0; i < nbr_column; i++)
                    {
                        std::string str;
                        std::getline(ss,str,';');
                        data[i].column.push_back(str);
                    }
                    j++;
                }
            }
            nll=j-1;
        }
        else
        {
            std::cout << "There is no file"<<std::endl;
            return;
        }
        /* Removing the '\n' from the end of data lines */
        if(headers[headers.size()-1][headers[headers.size()-1].length()-1]=='\n'){
            headers[headers.size()-1].erase(headers[headers.size()-1].length()-1);
            headers[headers.size()-1].shrink_to_fit();
        }
        for(int j=0;j<headers.size();j++) {
            data[j].header=headers[j];
        }
        for(int i=0;i<data[data.size()-1].column.size();i++){
            data[data.size()-1].column[i].erase(data[data.size()-1].column[i].length()-1);
            data[data.size()-1].column[i].shrink_to_fit();
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * *  : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    
    

    std::cout<<"Complete read_data ..."<<std::endl;

}

void datamonthly_average(int indx){
    std::vector<datta> monthly_average(5);
    monthly_average[0].header = "date_begin";
    monthly_average[1].header = "date_end";
    monthly_average[2].header = "sum_data";
    monthly_average[3].header = "nbr_days";
    //monthly_average[4].header = "nbr_samples";
    monthly_average[4].header = "data";

    //************************//
        //select the indexes of both of min date and the max date.
    int min = 0;
    int max = 0;
    for (int i = 0; i < data[index_date_begin].column.size(); i++)
    {
        if (date_to_num(data[index_date_begin].column[min]) > date_to_num(data[index_date_begin].column[i])){
            min = i;
        }
        if (date_to_num(data[index_date_begin].column[max]) < date_to_num(data[index_date_begin].column[i])){
            max = i;
        }
    }    
    //std::string date_b, date_e = data[index_date_begin].column[min];
    std::string date_b, date_e;
    //std::string date = data[index_date_begin].column[min];
    std::stringstream ss(data[index_date_begin].column[min]);
    std::string D,M,Y;
    std::getline(ss,Y,'-');
    std::getline(ss,M,'-');
    std::getline(ss,D,' ');
    int day = stoi(D), month=stoi(M), year=stoi(Y);
    std::string str_day, str_month, str_year,str_day1, str_month1, str_year1;
    std::stringstream ss_month, ss_month1, ss_year, ss_year1;
    ss_month<<month;
    ss_year<<year;
    if(month == 12){
        ss_month1 << 1;
        ss_year1 << year+1;
    }else{
        ss_month1 << month+1;
        ss_year1 << year;
    }
    ss_month >> str_month;
    ss_year >> str_year;
    ss_month1 >> str_month1;
    ss_year1 >> str_year1;
    date_b = str_year + "-" + str_month + "-" + "01 00:00";
    date_e = str_year1 + "-" + str_month1 + "-" + "01 00:00";
    bool date_series_is_over=true;

    if(date_to_num(date_e) < date_to_num(data[index_date_begin].column[max])){
        date_series_is_over = false;

    }
    std::cout<< date_e <<"            " <<date_to_num(date_e) << std::endl;
    
    while (date_series_is_over == false){
        monthly_average[0].column.push_back(date_b);
        monthly_average[1].column.push_back(date_e);
        date_b = date_e;
        std::string D1,M1,Y1;
        std::stringstream sss(date_b);
        std::stringstream ss_month2, ss_year2;
        std::getline(sss,Y1,'-');
        std::getline(sss,M1,'-');
        std::getline(sss,D1,' ');
        if(stoi(M1) == 12){
            ss_month2 << 1;
            ss_year2 << stoi(Y1)+1;
        }else{
            ss_month2 << stoi(M1)+1;
            ss_year2 << stoi(Y1);
        }
        ss_month2 >> str_month;
        ss_year2 >> str_year;
        date_e = str_year + "-" + str_month + "-" + "01 00:00";
        if (date_to_num(date_e) > date_to_num(data[index_date_begin].column[max])){
            monthly_average[0].column.push_back(date_b);
            monthly_average[1].column.push_back(date_e);
            date_series_is_over = true;
        }
    }
    monthly_average[2].column.resize(monthly_average[0].column.size());
    monthly_average[3].column.resize(monthly_average[0].column.size());
    //monthly_average[4].column.resize(monthly_average[0].column.size());
    monthly_average[4].column.resize(monthly_average[0].column.size());
    for (int i = 0; i < monthly_average[0].column.size(); i++)
    {
        monthly_average[2].column[i] = "0";
        monthly_average[3].column[i] = "0";
         //monthly_average[4].column[i] = "0";
        monthly_average[4].column[i] = "0";
    }
    std::cout<<"size    "<<monthly_average[4].column.size()<<std::endl;
    std::cout<<monthly_average[0].header<<"    "<<monthly_average[1].header<<std::endl;
    std::cout<<"**************************"<<std::endl;
    std::cout<<monthly_average[0].column.size()<<"     "<<monthly_average[1].column.size()<<std::endl;

    for (int i = 0; i < data[index_date_begin].column.size(); i++)
    {
        for (int j = 0; j < monthly_average[0].column.size(); j++)
        {
            if (date_to_num(data[index_date_begin].column[i]) >= date_to_num(monthly_average[0].column[j]) & date_to_num(data[index_date_begin].column[i])<date_to_num(monthly_average[1].column[j])){
                
                if (date_to_num(data[index_date_end].column[i]) < date_to_num(monthly_average[1].column[j])){
                    int n;
                    std::string ctr,nbr_day,sum_ctr,nbr_samp;
                    std::stringstream s_ctr,s_nbr_day,s_sum_ctr,s_nbr_samp;
                    n = date_to_num(data[index_date_end].column[i]) - date_to_num(data[index_date_begin].column[i]);
                    s_sum_ctr << stod(monthly_average[2].column[j])+ n*stod(data[indx].column[i]); //index_concentration
                    s_nbr_day << stoi(monthly_average[3].column[j])+n;
                    s_sum_ctr >> monthly_average[2].column[j];
                    s_nbr_day >> monthly_average[3].column[j];                    
                }else{
                    int n,n1;
                    std::string ctr,nbr_day,sum_ctr,nbr_samp;
                    std::stringstream s_ctr,s_nbr_day,s_sum_ctr,s_nbr_samp;
                    n = date_to_num(monthly_average[1].column[j]) - date_to_num(data[index_date_begin].column[i]);
                    n1 =date_to_num(data[index_date_end].column[i]) - date_to_num(monthly_average[1].column[j]);
                    s_sum_ctr << stod(monthly_average[2].column[j])+ n*stod(data[indx].column[i]); //index_concentration
                    s_nbr_day << stoi(monthly_average[3].column[j])+n;
                    s_sum_ctr >> monthly_average[2].column[j];
                    s_nbr_day >> monthly_average[3].column[j];


                    if (n1<date_to_num(monthly_average[1].column[j+1])- date_to_num(monthly_average[0].column[j+1])){
                        std::string ctr1,nbr_day1,sum_ctr1,nbr_samp1;
                        std::stringstream s_ctr1,s_nbr_day1,s_sum_ctr1,s_nbr_samp1;
                        s_sum_ctr1 << stod(monthly_average[2].column[j+1])+ n1*stod(data[indx].column[i]); // index concentration
                        s_nbr_day1 << stoi(monthly_average[3].column[j+1])+n1;
                        s_sum_ctr1 >> monthly_average[2].column[j+1];
                        s_nbr_day1 >> monthly_average[3].column[j+1];
                    }else{
                        bool b = false;
                        int x=1,y=0;
                        while (b == false)
                        {
                            y = date_to_num(monthly_average[1].column[j+x])- date_to_num(monthly_average[0].column[j+x]);
                            std::string ctr1,nbr_day1,sum_ctr1,nbr_samp1;
                            std::stringstream s_ctr1,s_nbr_day1,s_sum_ctr1,s_nbr_samp1;
                            s_sum_ctr1 << stod(monthly_average[2].column[j+x])+ y*stod(data[indx].column[i]);// index concentration
                            s_nbr_day1 << stoi(monthly_average[3].column[j+x])+y;
                            s_sum_ctr1 >> monthly_average[2].column[j+x];
                            s_nbr_day1 >> monthly_average[3].column[j+x];
                            x++;
                            n1=n1-y;
                            if (n1<date_to_num(monthly_average[1].column[j+x])- date_to_num(monthly_average[0].column[j+x])){
                                std::string ctr3,nbr_day3,sum_ctr3,nbr_samp3;
                                std::stringstream s_ctr3,s_nbr_day3,s_sum_ctr3,s_nbr_samp3;
                                s_sum_ctr3 << stod(monthly_average[2].column[j+x])+ n1*stod(data[indx].column[i]);// index concentration
                                s_nbr_day3 << stoi(monthly_average[3].column[j+x])+n1;
                                s_sum_ctr3 >> monthly_average[2].column[j+x];
                                s_nbr_day3 >> monthly_average[3].column[j+x];
                                b=true;
                            }
                        }
                    }
                }
            }
        }
        
    }
    for (int i = 0; i < monthly_average[0].column.size(); i++)
    {
        
        std::stringstream st;
        st<< stod(monthly_average[2].column[i])/stoi(monthly_average[3].column[i]);
        st>>monthly_average[4].column[i];
        std::cout<<monthly_average[0].column[i]<<"   "<<monthly_average[1].column[i]<<"   "<<monthly_average[4].column[i]<<std::endl;
    }
    
    //************   ************//
    std::ofstream data_moy;
    std::stringstream streamm;
    std::string str;
    streamm << data[indx].header;
    streamm >> str;
    data_moy.open("../Results/"+ str +"_mth_avrg_.csv");
    for (int i = 0; i < monthly_average.size(); i++)
    {   
        if (i == monthly_average.size()-1){
            data_moy << monthly_average[i].header<<std::endl;
        }else{
            data_moy<< monthly_average[i].header<<";";
        }
    }
    for (int i = 0; i < monthly_average[0].column.size(); i++){
        for (int j = 0; j < monthly_average.size(); j++){
            if (j==monthly_average.size()-1){
                data_moy<<monthly_average[j].column[i]<<std::endl;
            }else{
                data_moy<<monthly_average[j].column[i]<<";";
            }
        }
    }
}

void date_format(std::string date_column, std::string date_format){
    std::cout<<"Started Date_format ..."<<std::endl;
    int date;
    try
    {
        if (date_column == "begin"){
            date = index_date_begin;
        }else if (date_column == "end"){
            date = index_date_end;
        }else{
            date = get_index_data_headers(date_column, "else");
        }
        if (date == index_date_begin && date_begin_converted ==true){
            std::cout <<"'date begin' is already converted"<<std::endl;
            return;
        }else if (date == index_date_end && date_end_converted == true){
            std::cout <<"'date end' is already converted"<<std::endl;
            return;
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * an 'Error' has occured in function date_format:"<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    
    
    
    //std::string date="2001/31/01"; 
    for(int i=0;i<data[date].column.size();i++){
        std::string D,M,Y,h,m,s;
        std::stringstream ss(data[date].column[i]);
        char sep1='/',sep2=':',sep3=' ',sep='-';
        //std::string date_format="YYYY/DD/MM HH:mm:ss";
        if(date_format==""){
            std::cout<<"* * * Error * * * :"<<std::endl;
            std::cout<<"     Sorry! You have to Choose a date format"<<std::endl;
            return;
        }else if(date_format=="DD/MM/YYYY HH:mm:ss"){
            std::getline(ss,D,sep1);
            std::getline(ss,M,sep1);
            std::getline(ss,Y,sep3);
            std::getline(ss,h,sep2);
            if(h==""){h="0"; }
            std::getline(ss,m,sep2);
            if(m==""){m="00"; }
            // std::getline(ss,s);
        }else if(date_format=="MM/DD/YYYY HH:mm:ss"){
            std::getline(ss,M,sep1);
            std::getline(ss,D,sep1);
            std::getline(ss,Y,sep3);
            std::getline(ss,h,sep2);
            if(h==""){h="0"; }
            std::getline(ss,m,sep2);
            if(m==""){m="00"; }
            //std::getline(ss,s);
        }else if(date_format=="YYYY/DD/MM HH:mm:ss") {
            std::getline(ss,Y,sep1);
            std::getline(ss,D,sep1);
            std::getline(ss,M,sep3);
            std::getline(ss,h,sep2);
            if(h==""){h="0"; }
            std::getline(ss,m,sep2);
            if(m==""){m="00"; }
            //std::getline(ss,s);
        }else if(date_format=="YYYY/MM/DD HH:mm:ss"){
            std::getline(ss,Y,sep1);
            std::getline(ss,M,sep1);
            std::getline(ss,D,sep3);
            std::getline(ss,h,sep2);
            if(h==""){h="0"; }
            std::getline(ss,m,sep2);
            if(m==""){m="00"; }
            //std::getline(ss,s);
        }else if(date_format=="DD-MM-YYYY HH:mm:ss"){
            std::getline(ss,D,sep);
            std::getline(ss,M,sep);
            std::getline(ss,Y,sep3);
            std::getline(ss,h,sep2);
            if(h==""){h="0"; }
            std::getline(ss,m,sep2);
            if(m==""){m="00"; }
            //std::getline(ss,s);
        }else if(date_format=="MM-DD-YYYY HH:mm:ss"){
            std::getline(ss,M,sep);
            std::getline(ss,D,sep);
            std::getline(ss,Y,sep3);
            std::getline(ss,h,sep2);
            if(h==""){h="0"; }
            std::getline(ss,m,sep2);
            if(m==""){m="00"; }
            //std::getline(ss,s);
        }else if(date_format=="YYYY-DD-MM HH:mm:ss") {
            std::getline(ss,Y,sep);
            std::getline(ss,D,sep);
            std::getline(ss,M,sep3);
            std::getline(ss,h,sep2);
            if(h==""){h="0"; }
            std::getline(ss,m,sep2);
            if(m==""){m="00"; }
            //std::getline(ss,s);
        }else if(date_format=="YYYY-MM-DD HH:mm:ss"){
            std::getline(ss,Y,sep);
            std::getline(ss,M,sep);
            std::getline(ss,D,sep3);
            std::getline(ss,h,sep2);
            if(h==""){h="0"; }
            std::getline(ss,m,sep2);
            if(m==""){m="00"; }
            //std::getline(ss,s);
            //std::cout<<stoi(Y)<<sep<<stoi(M)<<sep<<stoi(D)<<std::endl;
        }else{
            std::cout<< "This format is not available"<<std::endl;
            std::cout<< "Your date have to be in the one of these formats :"<<std::endl;
            std::cout<<"1:     DD-MM-YYYY HH:mm:ss"<<std::endl;
            std::cout<<"2:     MM-DD-YYYY HH:mm:ss"<<std::endl;
            std::cout<<"3:     YYYY-MM-DD HH:mm:ss"<<std::endl;
            std::cout<<"4:     YYYY-DD-MM HH:mm:ss"<<std::endl;
            std::cout<<"5:     DD/MM/YYYY HH:mm:ss"<<std::endl;
            std::cout<<"6:     MM/DD/YYYY HH:mm:ss"<<std::endl;
            std::cout<<"7:     YYYY/MM/DD HH:mm:ss"<<std::endl;
            std::cout<<"8:     YYYY/DD/MM HH:mm:ss"<<std::endl;
        }
        try
        {
            ss<<stoi(Y);
            ss>>Y;
            ss<<stoi(D);
            ss>>D;
            ss<<stoi(M);
            ss>>M;
            ss<<stoi(h);
            ss>>h;
            data[date].column[i]=Y+"-"+M+"-"+D+" "+h+":"+m;
        }
        catch(const std::exception& e)
        {
            std::cout<<"* * * Error * * * : "<<std::endl;
            std::stringstream s_err;
            std::string error;
            s_err << e.what()<<std::endl;
            s_err >> error;
            std::cout<< error<<std::endl;
            std::cout<<"     You didn't choose the correct date format!"<<std::endl;
            std::cout<<"     You must to re-read the data and choose the correct date format!"<<std::endl;
            return;
        }
    }
    if (date == index_date_begin){
            date_begin_converted =true;
    }else if (date == index_date_end){
            date_end_converted = true;
    }
    std::cout<<"Complete date_format ..."<<std::endl;
}

void select_trajs_ctr(double min, double max, std::string path_dir){
	// std::filesystem::copy("/home/ana/PhD/Project/Data/Meteo/azazaz.csv", "/home/ana/PhD/Project/Data");
    try
    {
        for (int i=0; i<n_traj; i++){
            if (trajectory[i].ctr>=min && trajectory[i].ctr<max){
                std::filesystem::copy(listfiles[i],path_dir);
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
}

void concentration_trajectories(std::string eend){
    std::cout<<"Started Concentration_trajectories ..."<<std::endl;
    try
    {
        if (data.size()==0){
            std::cout<<"Sorry! You must first insert the data. "<<std::endl;
        }
        
        std::vector<int> data_date;
        for(int i=0;i<data[index_date_begin].column.size();i++){
            data_date.push_back(date_to_num(data[index_date_begin].column[i]));
        }
        data_date.push_back(date_to_num(eend));
        std::cout<<data[index_date_begin].column.size()<<"      "<<data_date.size()<<"******"<<std::endl;
        for(int i=0;i<n_traj;i++){
            for(int j=0;j<data[index_date_begin].column.size()-1;j++){
                if(data_date[j]<=trajectory[i].date_num && data_date[j+1]>=trajectory[i].date_num){
                    trajectory[i].ctr=stod(data[index_concentration].column[j]);
                        
                } 
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    
    
    std::cout<<"Complete Concentration_trajectories ..."<<std::endl;
}

void ctr_trajs(double missing_value){
    std::cout<<"Started Ctr_Trajs ..."<<std::endl;
    try
    {
        missing_values=missing_value;
        if (data.size()==0){
            std::cout<<"Sorry! You must first insert the data. "<<std::endl;
            return;
        }
        std::cout<<"============"<<std::endl;
        for (int i=0;i<n_traj;i++){
            trajectory[i].ctr= missing_values;
        }
        std::vector<int> data_date_begin,data_date_end;
        for(int i=0;i<data[index_date_begin].column.size();i++){
            data_date_begin.push_back(date_to_num(data[index_date_begin].column[i]));
            data_date_end.push_back(date_to_num(data[index_date_end].column[i]));
        }
        for(int i=0;i<n_traj;i++){
            for(int j=0;j<data[index_date_begin].column.size();j++){
                if(data_date_begin[j]<=trajectory[i].date_num && data_date_end[j]>trajectory[i].date_num){
                    try
                    {
                        trajectory[i].ctr=stod(data[index_concentration].column[j]);
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                        std::cout<<"* * * Warrning !* * * : "<<std::endl;
                        std::cout<<"There is a missing value between "<<'"'<<data[index_date_begin].column[j]<<'"'<<" and "<<'"'<<data[index_date_end].column[j]<<'"'<<"which has been replaced by :"<<missing_value<<std::endl;
                    }
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    std::cout<<"Complete Ctr_Trajs ..."<<std::endl;
}

void correlation(double left, double right, double bottom, double top, double sz, std::string para){
    std::cout<<"*****  Started Correlatiion *****"<<std::endl;
    try
    {
        int x=0;
        int y=0;
        int z=0;
        int index_para = -1;
        nc = int((right-left)/sz);
        nr = int((top - bottom)/sz);
        if (celle.size()!=0){
            celle.resize(0);
            celle.shrink_to_fit();
        }
        celle.resize(nc);
        for(int i=0;i<nc;i++){
            celle[i].resize(nr);
            for(int j=0;j<nr;j++){
                celle[i][j].resize(1);
            }
        }
        for (int i =0 ; i<n_traj;i++){
            index_para = -10;
            if (trajectory[i].ctr==missing_values){
                continue;
            }
            if (para=="Altitude"){
                index_para = -5;
            }else{
                for(int t = 0; t<trajectory[i].meteo_para.size(); t++){
                    if (trajectory[i].meteo_para[t].header==para){
                        index_para=t;
                        break;
                    }
                }
            }
//            std::cout<<"index_para" << index_para << std::endl;

            if (index_para == -10)
            {
                std::cout<<"Warning"<<std::endl;
                std::cout<<"The trajectory file"<<" '"<<trajectory[i].date <<"' "<<"does not contain any information on the "<<para<<std::endl;
                continue;
            }
            for (int j = 0; j < trajectory[i].lat.size(); j++){
                if (trajectory[i].lon[j] >= left && trajectory[i].lon[j]<right && trajectory[i].lat[j]>=bottom && trajectory[i].lat[j]<top){
                    int n=0;
                    for (double lo = left ; lo < right; lo = lo + sz ){ 	// lo == longitude 
                        if (trajectory[i].lon[j] >=lo && trajectory[i].lon[j] < lo+sz ){
                            x=n;
                            break;
                        }
                        n++;
                    }
                    int m=0;
                    for (double la = bottom ; la < top; la = la + sz ){	// la == latitude 
                        if (trajectory[i].lat[j]>=la && trajectory[i].lat[j]<la+sz ){
                            y=m;
                            break;
                        }
                        m++;
                    }
                    celle[x][y][z].ctr.push_back(trajectory[i].ctr);
                    if (para == "Altitude"){
                        celle[x][y][z].parameter.push_back(trajectory[i].alt[j]);
                    }else{
                        celle[x][y][z].parameter.push_back(trajectory[i].meteo_para[index_para].column[j]);
                    }
                    celle[x][y][z].sumC=celle[x][y][z].sumC+trajectory[i].ctr;
                    if (para == "Altitude"){
                        celle[x][y][z].sumP=celle[x][y][z].sumP+trajectory[i].alt[j];
                    }else{
                        celle[x][y][z].sumP=celle[x][y][z].sumP+trajectory[i].meteo_para[index_para].column[j];
                    }
                    //tau++;		// the total nbr of endpoints in the grids
                }
            }
        }

        for(int i=0;i<nc;i++){
            for(int j=0;j<nr;j++){    
                if(celle[i][j][z].ctr.size()>1){
                    celle[i][j][z].moy_c=celle[i][j][z].sumC/celle[i][j][z].ctr.size();
                    celle[i][j][z].moy_p=celle[i][j][z].sumP/celle[i][j][z].parameter.size();
                    double sd_c=0, sd_p=0;
                    for (int n = 0; n < celle[i][j][z].ctr.size(); n++)
                    {
                        sd_c = sd_c + pow((celle[i][j][z].ctr[n]-celle[i][j][z].moy_c),2);
                        sd_p = sd_p + pow((celle[i][j][z].parameter[n]-celle[i][j][z].moy_p),2);
                    }
                    celle[i][j][z].sd_c = pow(sd_c/(celle[i][j][z].ctr.size()-1), 0.5);
                    celle[i][j][z].sd_p = pow(sd_p/(celle[i][j][z].parameter.size()-1), 0.5);
                    double s=0;                                        //s=(xi-x)(yi-y)
                    //std::cout<<"******** size ctr="<<celle[i][j][z].ctr.size()<<" **************"<<"size para= "<<celle[i][j][z].parameter.size()<<"***********"<<std::endl;
                    for (int n = 0; n < celle[i][j][z].ctr.size(); n++)
                    {
                        s=s + (celle[i][j][z].ctr[n]-celle[i][j][z].moy_c)*(celle[i][j][z].parameter[n]-celle[i][j][z].moy_p);
                        //std::cout<<"c = "<<celle[i][j][z].ctr[n]<<"      moy_c"<<celle[i][j][z].moy_c<<"   par="<<celle[i][j][z].parameter[n]<<"   moy_p="<<celle[i][j][z].moy_p<<std::endl;
                        //std::cout<<"  c =   "<<celle[i][j][z].ctr[n]<<"         par ="<<celle[i][j][z].parameter[n]<<std::endl;
                    }
                    if(celle[i][j][z].sd_c*celle[i][j][z].sd_p !=0 ){
                        celle[i][j][z].r= 1./(celle[i][j][z].ctr.size()-1)*s/(celle[i][j][z].sd_c*celle[i][j][z].sd_p);
                    }
                    //std::cout<<" moy_c ="<<celle[i][j][z].moy_c<<"   sd_c ="<<celle[i][j][z].sd_c<<"    moy_p="<<celle[i][j][z].moy_p<<"    sd_p"<<celle[i][j][z].sd_p<<"      s="<<s<<std::endl;
                }
            }
        }
        std::ofstream out;
        out.open("../Results/Correlation/correlation.csv");
        out<<"correlation"<<";"<<"lat"<<";"<<"lon"<<";"<<"number_of_endpoints_in_cells"<<std::endl;
        double lo=bottom;
        for(int i=0;i<nr;i++){
            double la=left;
            for(int j=0;j<nc;j++){
                out<<celle[j][i][z].r<<";"<<la<<";"<<lo<<";"<<celle[j][i][z].ctr.size()<<std::endl;
                la=la+sz;
            }
            lo=lo+sz;
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    std::cout<<"Complete correlation"<<std::endl;
}

void CWT(double left, double right, double bottom, double top, double sz)
{   // top= 70 ; bottom=20; right= 20 ; left = -65
    std::cout<<"*****  Started CWT *****"<<std::endl;
    try
    {
        std::chrono::time_point<std::chrono::system_clock>start,end;
        start= std::chrono::system_clock::now();
        int tau=0;
        int x=0;
        int y=0;
        int z=0;
        
        nc = int((right-left)/sz);
        nr = int((top - bottom)/sz);
        celle.resize(0);
        celle.shrink_to_fit();
        celle.resize(nc);
        for(int i=0;i<nc;i++){
            celle[i].resize(nr);
            for(int j=0;j<nr;j++){
                celle[i][j].resize(1);
            }
        }
        for (int i =0 ; i<n_traj;i++){
            if (trajectory[i].ctr==missing_values){
                continue;
            }
            for (int j = 0; j < trajectory[i].lat.size(); j++){
                if (trajectory[i].lon[j] >= left && trajectory[i].lon[j]<right && trajectory[i].lat[j]>=bottom && trajectory[i].lat[j]<top){
                    int n=0;
                    for (double lo = left ; lo < right; lo = lo + sz ){ 	// lo == longitude 
                        if (trajectory[i].lon[j] >=lo && trajectory[i].lon[j] < lo+sz ){
                            x=n;
                            break;
                        }
                        n++;
                    }
                    
                    int m=0;
                    for (double la = bottom ; la < top; la = la + sz ){	// la == latitude 
                        if (trajectory[i].lat[j]>=la && trajectory[i].lat[j]<la+sz ){
                            y=m;
                            break;
                        }
                        m++;
                    }    
                    celle[x][y][z].duration++;
                    celle[x][y][z].sumC=celle[x][y][z].sumC+trajectory[i].ctr;
                    tau++;		// the total nbr of endpoints in the grids 
                    bool exist =false;
                    for(int t=0;t<celle[x][y][z].tr.size();t++){
                        if (celle[x][y][z].tr[t]==i){
                            exist=true;
                        }
                    }
                    if(exist==false){
                        celle[x][y][z].tr.push_back(i);
                    }
                }
            }
        }
        for(int i=0;i<nc;i++){
            for(int j=0;j<nr;j++){    
                if(celle[i][j][z].duration!=0){
                    celle[i][j][z].cwt=(celle[i][j][z].sumC/celle[i][j][z].duration);
                }
            }
        }
        std::ofstream out;
        out.open("../Results/CWT/cwt.csv");
        out<<"cwt"<<";"<<"lat"<<";"<<"lon"<<std::endl;
        double lo=bottom;
        for(int i=0;i<nr;i++){
            double la=left;
            for(int j=0;j<nc;j++){
                out<<celle[j][i][z].cwt<<";"<<la<<";"<<lo<<std::endl;
                la=la+sz;
            }
            lo=lo+sz;
        }
        std::cout<<"tau"<<" ="<<tau<<"   tau/nbr de cells =  "<<double(tau/(nc*nr))<<std::endl;
        end= std::chrono::system_clock::now();
        long int microseconds =std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
        std::cout<<"calculation time in microseconds "<<microseconds<<std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    std::cout<<"Complete CWT_3D"<<std::endl;
}

void WCWT(std::string reduce_ratio, std::string weights, double left, double bottom, double sz, std::string weight){
    try
    {
        std::vector<double> ratio;	 // weights "30;10;5" ;  wt =[30;10;5;0]  ;  ratio=[0.7;0.5;0.1] weight = endpints or trajectory
        std::vector<int> wt;
    
        std::stringstream ss1(reduce_ratio);
        std::string str;
        while (std::getline(ss1, str, ';'))
        {
            ratio.push_back(stod(str));
        }

        std::stringstream ss2(weights);
        while (std::getline(ss2, str, ';'))
        {
            wt.push_back(stod(str));
        }

        int len_ratio=ratio.size();
        int len_weights= wt.size();
        if (len_ratio != len_weights){
            std::cout<<"Sorry! The lengths of 'Reduce Ratio' and 'Endpoints' must be equal."<<std::endl;
            return;
        }
        std::ofstream out;
        out.open("../Results/CWT/wcwt.csv");
        out<<"wcwt"<<";"<<"lat"<<";"<<"lon"<<std::endl;
        for(int i=0;i<nr;i++){
            for(int j=0;j<nc;j++){
                celle[j][i][0].wcwt=celle[j][i][0].cwt;
            }
        }
        wt.push_back(0);
        double lo=bottom;
        for(int i=0;i<nr;i++){
            double la=left;
            for(int j=0;j<nc;j++){
                if(weight=="trajectory"){  /////// weight by "endpoints" or "trajectory"
                    for (int n=wt.size()-1 ; n>0;n--){
                        if(celle[j][i][0].tr.size() < wt[n-1] && celle[j][i][0].tr.size() >= wt[n]){
                                celle[j][i][0].wcwt=celle[j][i][0].cwt*ratio[n-1];
                        }
                    }
                }else{
                    for (int n=wt.size()-1 ; n>0;n--){
                        if(celle[j][i][0].duration < wt[n-1] && celle[j][i][0].duration >= wt[n]){
                                celle[j][i][0].wcwt=celle[j][i][0].cwt*ratio[n-1];
                        }
                    }
                }
                out<<celle[j][i][0].wcwt<<";"<<la<<";"<<lo<<std::endl;
                la=la+sz;
            }
            lo=lo+sz;
        }
        out.close();
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
}

void CWT_3D(double left, double right, double bottom, double top, double sz, std::string heights)
{   // top= 70 ; bottom=20; right= 20 ; left = -65
    try
    {
        std::chrono::time_point<std::chrono::system_clock>start,end;
        start= std::chrono::system_clock::now();
        std::cout<<"*****  Started CWT *****"<<std::endl;
        std::vector<int> tau;
        int x=0;
        int y=0;
        int z=0;
        std::vector<double> levels;
        std::stringstream ss(heights);
        std::string str; 
        levels.resize(1);
        levels[0]=0;
        while (std::getline(ss, str,';')){	
            if(stod(str)==0){
                continue;
            }
            levels.push_back(stod(str));
        }
        levels.push_back(1000000);
        
        nc = int((right-left)/sz);
        nr = int((top - bottom)/sz);
        nl = levels.size()-1;
        tau.resize(nl);
        celle.resize(0);
        celle.shrink_to_fit();
        celle.resize(nc);
        for(int i=0;i<nc;i++){
            celle[i].resize(nr);
            for(int j=0;j<nr;j++){
                celle[i][j].resize(nl);
            }
        }
        
        for (int i =0 ; i<n_traj;i++){
            if (trajectory[i].ctr==missing_values){
                continue;
            }
            for (int j = 0; j < trajectory[i].lat.size(); j++){
                if (trajectory[i].lon[j] >= left && trajectory[i].lon[j]<right && trajectory[i].lat[j]>=bottom && trajectory[i].lat[j]<top){
                    int n=0;
                    for (double lo = left ; lo < right; lo = lo + sz ){ 	// lo == longitude 
                        if (trajectory[i].lon[j] >=lo && trajectory[i].lon[j] < lo+sz ){
                            x=n;
                            break;
                        }
                        n++;
                    }
                    
                    int m=0;
                    for (double la = bottom ; la < top; la = la + sz ){	// la == latitude 
                        if (trajectory[i].lat[j]>=la && trajectory[i].lat[j]<la+sz ){
                            y=m;
                            break;
                        }
                        m++;
                    }
                    
                    for (int o = 0 ; o < nl; o++ ){
                        if (trajectory[i].alt[j]>=levels[o] && trajectory[i].alt[j]<levels[o+1] ){
                            z=o;
                            break;
                        }
                    }
                    
                    celle[x][y][z].duration++;
                    celle[x][y][z].sumC=celle[x][y][z].sumC+trajectory[i].ctr;
                    tau[z]++;		// the total nbr of endpoints in the grids 
                    bool exist =false;
                    for(int t=0;t<celle[x][y][z].tr.size();t++){
                        if (celle[x][y][z].tr[t]==i){
                            exist=true;
                        }
                    }
                    if(exist==false){
                        celle[x][y][z].tr.push_back(i);
                    }
                }
            }
        }

        for(int i=0;i<nc;i++){
            for(int j=0;j<nr;j++){
                for(int k = 0;k<nl;k++){        
                    if(celle[i][j][k].duration!=0){
                        celle[i][j][k].cwt=(celle[i][j][k].sumC/celle[i][j][k].duration);
                    }
                }
            }
        }
        std::vector<std::string> filename;
        std::vector<std::string> titel;
        std::vector<std::ofstream> out;
        filename.resize(nl);
        titel.resize(nl);
        out.resize(nl);
        
        
        std::string t0,t1,t2;
        for(int i=0;i<nl;i++){
            std::stringstream ss0,ss1,ss2;
            ss0<<i;
            ss1<<levels[i];
            ss2<<levels[i+1];
            ss0 >> t0;
            ss1 >> t1;
            ss2 >> t2;
            filename[i]="../Results/CWT/cwt"+t0+".csv";
            if(i!=nl-1){
                titel[i]="CWT for altitude bitween : "+t1+" and "+t2;
            }else{
                titel[i]="CWT for altitude highier than : "+t1;
            }
        }
        // out.open("../Results/CWT/cwt.csv");
        for(int k=0;k<nl;k++){
                out[k].open(filename[k]);
                out[k]<<"cwt"<<";"<<"lat"<<";"<<"lon"<<std::endl;
                double lo=bottom;
            for(int i=0;i<nr;i++){
                double la=left;
                for(int j=0;j<nc;j++){
                    out[k]<<celle[j][i][k].cwt<<";"<<la<<";"<<lo<<std::endl;
                    la=la+sz;
                }
                lo=lo+sz;
            }
        }
        std::cout<<" ________ resume CWT 3D________"<<std::endl;
        std::cout<<"*= heights = "<<heights<<std::endl;
        for(int i=0;i<nl;i++){
            std::cout<<levels[i]<<"       ";
        }
        std::cout<<std::endl;
        std::cout<<"*= tau : "<<std::endl;
        for(int i=0;i<nl;i++){
            std::cout<<tau[i]<<"           ";
        }
        std::cout<<std::endl;
        std::cout<<"*= filename  : "<<std::endl;;
        for(int i=0;i<nl;i++){
            std::cout<<"    "<<filename[i]<<std::endl;
        }
        std::cout<<std::endl;
        std::cout<<"*= tilel  : "<<std::endl;;
        for(int i=0;i<nl;i++){
            std::cout<<"    "<<titel[i]<<std::endl;
        }
        std::cout<<std::endl<<std::endl;

        for(int i=0;i<nl;i++){
            std::cout<<"tau"<<i<<" ="<<tau[i]<<"   tau/nbr de cells =  "<<tau[i]/(nc*nr)<<std::endl;
        }
        end= std::chrono::system_clock::now();
        long int microseconds =std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
        std::cout<<"calculation time in microseconds "<<microseconds<<std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    std::cout<<"Complete CWT_3D"<<std::endl;
}

void WCWT_3D(std::string reduce_ratios, std::string weightss, double left, double bottom, double sz, std::string weight){
    try
    {
        std::stringstream ss(reduce_ratios);
        std::stringstream ss1(weightss);
        std::vector<std::string> reduce_ratio;
        std::vector<std::string> weights;
        std::string str, str1;
        while (std::getline(ss, str, ':'))
        {
            reduce_ratio.push_back(str);
        }
        while (std::getline(ss1, str1, ':'))
        {
            weights.push_back(str1);
        }
        std::vector<std::vector<double>> ratio;	 // weights "30;10;5" ;  wt =[30;10;5;0]  ;  ratio=[0.7;0.5;0.1] weight = endpints or trajectory
        std::vector<std::vector<int>> wt;
        wt.resize(weights.size());
        ratio.resize(reduce_ratio.size());
        for (int i=0;i<ratio.size();i++){
            std::stringstream ss2(reduce_ratio[i]);
            std::string str2;
            while (std::getline(ss2, str2, ';'))
            {
            ratio[i].push_back(stod(str2));
            }
        }

        for (int i=0;i<wt.size();i++){
            std::stringstream ss2(weights[i]);
            std::string str2;
            while (std::getline(ss2, str2, ';'))
            {
                wt[i].push_back(stod(str2));
            }
        }
        
        int len_ratio=ratio.size();
        int len_weights= wt.size();
        if (len_ratio != len_weights){
            std::cout<<"Sorry! The lengths of 'Reduce Ratio' and 'Endpoints' must be equal."<<std::endl;
            return;
        }else{
            for (int i=0;i<len_ratio;i++){
                if (ratio[i].size()!=wt[i].size()){
                    std::cout<<"Sorry! The lengths of 'Reduce Ratio' and 'Endpoints' must be equal."<<std::endl;
                    return;
                }
            }
        }
        std::vector<std::ofstream> out;
        out.resize(nl);
        for (int k=0; k<nl; k++){
            std::string num;
            std::stringstream sss;
            sss<<k;
            sss>>num;
            out[k].open("../Results/CWT/wcwt"+num+".csv");
            out[k]<<"wcwt"<<";"<<"lat"<<";"<<"lon"<<std::endl;
            for(int i=0;i<nr;i++){
                for(int j=0;j<nc;j++){
                    celle[j][i][k].wcwt=celle[j][i][k].cwt;
                }
            }
            wt[k].push_back(0);
            double lo=bottom;
            for(int i=0;i<nr;i++){
            double la=left;
            for(int j=0;j<nc;j++){
                if(weight=="trajectory"){  /////// weight by "endpoints" or "trajectory"
                    for (int n=wt[k].size()-1 ; n>0;n--){
                        if(celle[j][i][k].tr.size() < wt[k][n-1] && celle[j][i][k].tr.size() >= wt[k][n]){
                            celle[j][i][k].wcwt=celle[j][i][k].cwt*ratio[k][n-1];
                        }
                    }
                }else{
                    for (int n=wt[k].size()-1 ; n>0;n--){
                        if(celle[j][i][k].duration < wt[k][n-1] && celle[j][i][k].duration >= wt[k][n]){
                            celle[j][i][k].wcwt=celle[j][i][k].cwt*ratio[k][n-1];
                        }
                    }
                }
                out[k]<<celle[j][i][k].wcwt<<";"<<la<<";"<<lo<<std::endl;
                la=la+sz;
            }
            lo=lo+sz;
            
            }
            out[k].close();
            
        }


        std::cout<<" ________ resume WCWT 3D________"<<std::endl;
        std::cout<<"*= reduce_ratios    =      "<<reduce_ratios<<std::endl;
        std::cout<<"* nl === "<<nl<<std::endl;
        for(int i=0;i<nl;i++){
            std::cout<<"  reduce_ratio[i].size() = "<<reduce_ratio[i].size()<<std::endl;
            std::cout<<reduce_ratio[i]<<"       ";
            for(int j=0;j<ratio[i].size();j++){
                std::cout<<ratio[i][j]<<"      ";
            }
            std::cout<<std::endl;
        }
        
        std::cout<<"*= weightss     =     "<<weightss<<std::endl;
        for(int i=0;i<nl;i++){
            std::cout<<weights[i]<<"          ";
            for(int j=0;j<wt[i].size();j++){
                std::cout<<wt[i][j]<<"     ";
            }
            std::cout<<std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
}

/*void showCWT(){
    std::ofstream out;
    out.open("../Results/CWT/output_cwt.csv");
    out<<"N_traj"<<";"<<"duration"<<";"<<"CWT"<<";"<<"WCWT"<<std::endl;
    for (int){
	    for(int i=0;i<nr;i++){
            for(int j=0;j<nc;j++){
                out<<celle[j][i].tr.size()<<";"<<celle[j][i].duration<<";"<<celle[j][i].cwt<<";" <<celle[j][i].wcwt<<std::endl;
            }
	    }
    }
}*/

void PSCF(double left, double right, double bottom, double top, double sz, double criterion){
    /*    Nij == cell.duration   ;;   nij == cell.n    ;;     PSCF=cell.n/cell.duration    */
    try
    {
        std::chrono::time_point<std::chrono::system_clock>start,end;
        start= std::chrono::system_clock::now();
        std::cout<<"Started PSCF ..."<<std::endl;
        int tau=0;
        int x=0; 
        int y=0;
        int z=0;
        nc = (right-left)/sz;     //? sz=0.3         /// x
        nr = (top - bottom)/sz;
        celle.resize(0);
        celle.shrink_to_fit();
        celle.resize(nc);
        for(int i=0;i<nc;i++){				// reisze(4,3,2)
            celle[i].resize(nr);
            for(int j=0;j<nr;j++){
                celle[i][j].resize(1);
            }
        }
        
        for (int i =0 ; i<n_traj;i++){
            if (trajectory[i].ctr==missing_values){
                continue;
            }
            for (int j = 0; j < trajectory[i].lat.size(); j++){
                if (trajectory[i].lon[j] >= left && trajectory[i].lon[j]<right && trajectory[i].lat[j]>=bottom && trajectory[i].lat[j]<top){
                    int n=0;
                    for (double lo = left ; lo < right; lo = lo + sz ){
                        if (trajectory[i].lon[j] >=lo && trajectory[i].lon[j] < lo+sz){
                            x=n;
                            break;
                        }
                        n++;
                    }
                    int m=0;
                    for (double la = bottom ; la < top; la = la + sz ){
                        if (trajectory[i].lat[j]>=la && trajectory[i].lat[j]<la+sz){
                            y=m;
                            break;
                        }
                        m++;
                    }

                    celle[x][y][0].duration++;
                    if(trajectory[i].ctr>=criterion){       // test
                        celle[x][y][0].n++;
                    }
                    tau++;
                    bool exist =false;
                    for(int t=0;t<celle[x][y][0].tr.size();t++){
                        if (celle[x][y][0].tr[t]==i){
                            exist=true;
                        }
                    }
                    if(exist==false){
                        celle[x][y][0].tr.push_back(i);
                    }
                }
            }
        }
        for(int i=0;i<nc;i++){
            for(int j=0;j<nr;j++){
                if(celle[i][j][0].duration!=0)
                    celle[i][j][0].pscf=(celle[i][j][0].n/double(celle[i][j][0].duration));  
            }
        }
        std::ofstream out;
        out.open("../Results/PSCF/pscf.csv");
        out<<"pscf"<<";"<<"lat"<<";"<<"lon"<<std::endl;
        double lo=bottom;
        for(int i=0;i<nr;i++){
            double la=left;
            for(int j=0;j<nc;j++){
                out<<celle[j][i][0].pscf<<";"<<la<<";"<<lo<<std::endl;
                la=la+sz;
            }
            lo=lo+sz;
        }
        std::cout<<"tau"<<" ="<<tau<<"   tau/nbr de cells =  "<<tau/(nc*nr)<<std::endl;
        end= std::chrono::system_clock::now();
        long int microseconds =std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
        std::cout<<"calculation time in microseconds "<<microseconds<<std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    std::cout<<"Complete PSCF"<<std::endl;
}

void WPSCF(std::string reduce_ratio, std::string weights, double left, double bottom, double sz, std::string weight){
    try
    {
        std::vector<double> ratio;	 // weights "30;10;5" ;  wt =[30;10;5;0]  ;  ratio=[0.7;0.5;0.1] weight = endpints or trajectory
        std::vector<int> wt;
        std::stringstream ss1(reduce_ratio);
        std::string str;
        std::cout<<"weight"<<std::endl;
        while (std::getline(ss1, str, ';'))
        {
            ratio.push_back(stod(str));
            std::cout<<str<<"   "<<ratio.size()<<std::endl;
        }
        std::stringstream ss2(weights);
        std::cout<<"weight"<<std::endl;
        while (std::getline(ss2, str, ';'))
        {
            wt.push_back(stod(str));
            std::cout<<str<<"   "<<wt.size()<<std::endl;
        }

        int len_ratio=ratio.size();
        int len_weights= wt.size();
        if (len_ratio != len_weights){
            std::cout<<"Sorry! The lengths of 'Reduce Ratio' and 'Endpoints' must be equal."<<std::endl;
            return;
        }
        std::ofstream out;
        out.open("../Results/PSCF/wpscf.csv");
        out<<"wpscf"<<";"<<"lat"<<";"<<"lon"<<std::endl;
        for(int i=0;i<nr;i++){
            for(int j=0;j<nc;j++){
                celle[j][i][0].wpscf=celle[j][i][0].pscf;
            }
        }
        wt.push_back(0);
        double lo=bottom;
        for(int i=0;i<nr;i++){
            double la=left;
            for(int j=0;j<nc;j++){
                if(weight=="trajectory"){  /////// weight by "endpoints" or "trajectory"
                    for (int n=wt.size()-1 ; n>0;n--){
                        if(celle[j][i][0].tr.size() < wt[n-1] && celle[j][i][0].tr.size() >= wt[n]){
                                celle[j][i][0].wpscf=celle[j][i][0].pscf*ratio[n-1];
                        }
                    }
                }else{
                    for (int n=wt.size()-1 ; n>0;n--){
                        if(celle[j][i][0].duration < wt[n-1] && celle[j][i][0].duration >= wt[n]){
                            celle[j][i][0].wpscf=celle[j][i][0].pscf*ratio[n-1];
                        }
                    }
                }
                out<<celle[j][i][0].wpscf<<";"<<la<<";"<<lo<<std::endl;
                la=la+sz;
            }
            lo=lo+sz;	
        }
        out.close();
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    
}

void PSCF_3D(double left, double right, double bottom, double top, double sz, std::string heights,double criterion){
    /*    Nij == cell.duration   ;;   nij == cell.n    ;;     PSCF=cell.n/cell.duration    */
    try
    {
        std::chrono::time_point<std::chrono::system_clock>start,end;
        start= std::chrono::system_clock::now();
        std::cout<<"Started PSCF ..."<<std::endl;
        std::vector<int> tau;
        int x=0; 
        int y=0;
        int z=0;
        std::vector<double> levels;
        std::stringstream ss(heights);
        std::string str; 
        levels.resize(1);
        levels[0]=0;
        while (std::getline(ss, str,';'))
        {	
            if(stod(str)==0){
                continue;
            }
            levels.push_back(stod(str));
        }
        levels.push_back(1000000);
        nc = (right-left)/sz;     //? sz=0.3         /// x
        nr = (top - bottom)/sz;   
        nl = levels.size()-1;
        tau.resize(nl);
        celle.resize(0);
        celle.shrink_to_fit();
        celle.resize(nc);
        for(int i=0;i<nc;i++){				// reisze(4,3,2)
            celle[i].resize(nr);
            for(int j=0;j<nr;j++){
                celle[i][j].resize(nl);
            }
        }
        for (int i =0 ; i<n_traj;i++){
            if (trajectory[i].ctr==missing_values){
                continue;
            }
            for (int j = 0; j < trajectory[i].lat.size(); j++){
                if (trajectory[i].lon[j] >= left && trajectory[i].lon[j]<right && trajectory[i].lat[j]>=bottom && trajectory[i].lat[j]<top){
                    int n=0;
                    for (double lo = left ; lo < right; lo = lo + sz ){
                        if (trajectory[i].lon[j] >=lo && trajectory[i].lon[j] < lo+sz ){
                            x=n;
                            break;
                        }
                        n++;
                    }
                    int m=0;
                    for (double la = bottom ; la < top; la = la + sz ){
                        if (trajectory[i].lat[j]>=la && trajectory[i].lat[j]<la+sz ){
                            y=m;
                            break;
                        }
                        m++;
                    }
                    
                    for (int o = 0 ; o < nl; o++ ){
                        if (trajectory[i].alt[j]>=levels[o] && trajectory[i].alt[j]<levels[o+1]){
                            z=o;
                            break;
                        }
                    }
                    celle[x][y][z].duration++;
                    if(trajectory[i].ctr>=criterion){       // test
                        celle[x][y][z].n++;
                    }
                    tau[z]++;
                    bool exist =false;
                    for(int t=0;t<celle[x][y][z].tr.size();t++){
                        if (celle[x][y][z].tr[t]==i){
                            exist=true;
                        }
                    }
                    if(exist==false){
                        celle[x][y][z].tr.push_back(i);
                    }
                }
            }
        }
        for(int i=0;i<nc;i++){
            for(int j=0;j<nr;j++){
                for(int k = 0;k<nl;k++){
                if(celle[i][j][k].duration!=0)
                    celle[i][j][k].pscf=(celle[i][j][k].n/double(celle[i][j][k].duration));
                }
            }
        }
        std::vector<std::string> filename;
        std::vector<std::string> titel;
        std::vector<std::ofstream> out;
        filename.resize(nl);
        titel.resize(nl);
        out.resize(nl);
        std::string t0,t1,t2;
        for(int i=0;i<nl;i++){
            std::stringstream ss0,ss1,ss2;
            ss0 << i;
            ss1 << levels[i];
            ss2 << levels[i+1];
            ss0 >> t0;
            ss1 >> t1;
            ss2 >> t2;
            filename[i]="../Results/PSCF/pscf"+t0+".csv";
            if(i!=nl-1){
                titel[i]="PSCF for altitude bitween : "+t1+" and "+t2;
            }else{
                titel[i]="PSCF for altitude highier than : "+t1;
            }
        }
        for(int k=0;k<nl;k++){
            out[k].open(filename[k]);
            out[k]<<"pscf"<<";"<<"lat"<<";"<<"lon"<<std::endl;
            double lo=bottom;
            for(int i=0;i<nr;i++){
                double la=left;
                for(int j=0;j<nc;j++){
                    out[k]<<celle[j][i][k].pscf<<";"<<la<<";"<<lo<<std::endl;
                    la=la+sz;
                }
                lo=lo+sz;
            }
        }
        std::cout<<" ________ resume PSCF 3D________"<<std::endl;
        std::cout<<"*= heights = "<<heights<<std::endl;
        for(int i=0;i<nl;i++){
            std::cout<<levels[i]<<"       ";
        }
        std::cout<<std::endl;
        std::cout<<"*= tau : "<<std::endl;
        for(int i=0;i<nl;i++){
            std::cout<<tau[i]<<"           ";
        }
        std::cout<<std::endl;
        std::cout<<"*= filename  : "<<std::endl;;
        for(int i=0;i<nl;i++){
            std::cout<<"    "<<filename[i]<<std::endl;
        }
        std::cout<<std::endl;
        std::cout<<"*= tilel  : "<<std::endl;;
        for(int i=0;i<nl;i++){
            std::cout<<"    "<<titel[i]<<std::endl;
        }
        std::cout<<std::endl;
        for(int i=0;i<nl;i++){
            std::cout<<"tau"<<i<<" ="<<tau[i]<<"   tau/nbr de cells =  "<<tau[i]/(nc*nr)<<std::endl;
        }
        end= std::chrono::system_clock::now();
        long int microseconds =std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
        std::cout<<"calculation time in microseconds "<<microseconds<<std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    std::cout<<"Complete PSCF"<<std::endl;
}

void WPSCF_3D(std::string reduce_ratios, std::string weightss, double left, double bottom, double sz, std::string weight){
    try
    {
        std::stringstream ss(reduce_ratios);
        std::stringstream ss1(weightss);
        std::vector<std::string> reduce_ratio;
        std::vector<std::string> weights;
        std::string str, str1;
        while (std::getline(ss, str, ':'))
        {
            reduce_ratio.push_back(str);
        }
        while (std::getline(ss1, str1, ':'))
        {
            weights.push_back(str1);
        }
        
        std::vector<std::vector<double>> ratio;	 // weights "30;10;5" ;  wt =[30;10;5;0]  ;  ratio=[0.7;0.5;0.1] weight = endpints or trajectory
        std::vector<std::vector<int>> wt;
        wt.resize(weights.size());
        ratio.resize(reduce_ratio.size());
        for (int i=0;i<ratio.size();i++){
            std::stringstream ss2(reduce_ratio[i]);
            std::string str2;
            while (std::getline(ss2, str2, ';'))
            {
                ratio[i].push_back(stod(str2));
            }
        }

        for (int i=0;i<wt.size();i++){
            std::stringstream ss2(weights[i]);
            std::string str2;
            while (std::getline(ss2, str2, ';'))
            {
                wt[i].push_back(stod(str2));
            }
        }
        
        int len_ratio=ratio.size();
        int len_weights= wt.size();
        if (len_ratio != len_weights){
            std::cout<<"Sorry! The lengths of 'Reduce Ratio' and 'Endpoints' must be equal."<<std::endl;
            return;
        }else{
            for (int i=0;i<len_ratio;i++){
                if (ratio[i].size()!=wt[i].size()){
                    std::cout<<"Sorry! The lengths of 'Reduce Ratio' and 'Endpoints' must be equal."<<std::endl;
                    return;
                }
            }
        }

        std::vector<std::ofstream> out;
        out.resize(nl);
        for (int k=0; k<nl; k++){
            std::string num;
            std::stringstream sss;
            sss<<k;
            sss>>num;
            out[k].open("../Results/PSCF/wpscf"+num+".csv");
            out[k]<<"wpscf"<<";"<<"lat"<<";"<<"lon"<<std::endl;
            for(int i=0;i<nr;i++){
                for(int j=0;j<nc;j++){
                    celle[j][i][k].wpscf=celle[j][i][k].pscf;
                }
            }
            wt[k].push_back(0);
            double lo=bottom;
            for(int i=0;i<nr;i++){
                double la=left;
                for(int j=0;j<nc;j++){
                    if(weight=="trajectory"){  /////// weight by "endpoints" or "trajectory"
                        for (int n=wt[k].size()-1 ; n>0;n--){
                            if(celle[j][i][k].tr.size() < wt[k][n-1] && celle[j][i][k].tr.size() >= wt[k][n]){
                                    celle[j][i][k].wpscf=celle[j][i][k].pscf*ratio[k][n-1];
                            }
                        }
                    }else{
                        for (int n=wt[k].size()-1 ; n>0;n--){
                            if(celle[j][i][k].duration < wt[k][n-1] && celle[j][i][k].duration >= wt[k][n]){
                                    celle[j][i][k].wpscf=celle[j][i][k].pscf*ratio[k][n-1];
                            }
                        }
                    }
                    out[k]<<celle[j][i][k].wpscf<<";"<<la<<";"<<lo<<std::endl;
                    la=la+sz;
                }
                lo=lo+sz;	
            }
            out[k].close();
        }
        std::cout<<" ________ resume WPSCF 3D________"<<std::endl;
        std::cout<<"*= reduce_ratios = "<<reduce_ratios<<std::endl;
        for(int i=0;i<nl;i++){
            std::cout<<reduce_ratio[i]<<"     ";
            for(int j=0;j<ratio[i].size();j++){
                std::cout<<ratio[i][j]<<"   ";
            }
            std::cout<<std::endl;
        }
        
        std::cout<<"*= weightss   =    "<<weightss<<std::endl;
        for(int i=0;i<nl;i++){
            std::cout<<weights[i]<<"          ";
            for(int j=0;j<wt[i].size();j++){
                std::cout<<wt[i][j]<<"     ";
            }
            std::cout<<std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    
}

void fun_TSV (std::vector<cluster> clust,int i,int s,int num_thread,long double tsv,int nh, int skip){
    try
    {
        double lat_moy_temp[nh]={0},lon_moy_temp[nh]={0};
        int xx =clust.size();
        double la[nh]={0},lo[nh]={0};
        long double csv=0.0;
        int c1=clust.size()+5,c2=clust.size()+5;
        for(;i<s;i++){ 
            for(int j=i+1; j<xx;j++){
                //  c a l c u l   d u   m o y e n   d u    c l u s t e r   t e m p o r a i r e  
                double si = clust[i].trajectories.size();
                double sj = clust[j].trajectories.size();
                for(int n=0;n<nh;n++){
                    lat_moy_temp[n]=(clust[i].lat_moy[n]*si+clust[j].lat_moy[n]*sj)/(si+sj);
                    lon_moy_temp[n]=(clust[i].lon_moy[n]*si+clust[j].lon_moy[n]*sj)/(si+sj);
                }
                // c a l c u l   d e   l a   d i s t a n c e   e u c l i d i e n n e   e t   c l s u t e r   s p a t i a l  v a r i a n c e   
                long double d=0.0;
                for (int n=0;n<clust[i].trajectories.size();n++){
                    for(int m=0;m<nh;m=m+skip){
                        d=d+pow(trajectory[clust[i].trajectories[n]].lat[m]-lat_moy_temp[m],2) + pow(trajectory[clust[i].trajectories[n]].lon[m]-lon_moy_temp[m],2);
                    }
                }
                for (int n=0;n<clust[j].trajectories.size();n++){       // we calculate d=dj+di because j and i becomes one cluster 
                    for(int m=0;m<nh;m=m+skip){
                        d=d+pow(trajectory[clust[j].trajectories[n]].lat[m]-lat_moy_temp[m],2)+pow(trajectory[clust[j].trajectories[n]].lon[m]-lon_moy_temp[m],2);//////x*x
                    }
                }
                long double tv=d;         //??????????????  
                for(int n=0;n<clust.size(); n++ ){
                    tv = tv + clust[n].csv;
                }
                tv =tv-clust[i].csv-clust[j].csv; // clust[i].csv-clust[j].csv====>d ;
                if (tv<tsv){
                    tsv= tv;
                    TV_th[num_thread].tsv=tsv;
                    TV_th[num_thread].c1=i;
                    TV_th[num_thread].c2=j;
                    TV_th[num_thread].csv=d;
                    for(int n=0;n<nh;n++){
                        TV_th[num_thread].la[n] = lat_moy_temp[n];  // la = sum_lat  : pour garder la valeur de sum_lat du cluster correspondant de min(tsv) , on va l'utiliser pour calculer les coordonnees de trajectoire moyen a la prochene fois
                        TV_th[num_thread].lo[n] = lon_moy_temp[n];
                    }
                }
                else if (tv==tsv){
                    std::cout<<e<<std::endl;
                    e++;
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    
}

void clustering_TSV (int nth, int skip){ //( int nh, int nth){
    try
    {
        if (nh == 0){
            std::cout<<"* * * Error * * *"<<std::endl;
            std::cout<<"    That is because  one of these reasons :"<<std::endl;
            std::cout<<"    1 - You don't read trajectories ."<<std::endl;
            std::cout<<"    2 - The trajectories don't have the same number of endpoints."<<std::endl;
            return;
        }
        std::ofstream out,meanclster;
        out.open("../Results/Cluster/clusters_TSV.txt");
        meanclster.open("../Results/Cluster/meanclsters_TSV.csv");
        std::cout<<"* * * clustering * * * "<<std::endl;
        double lat_moy_temp[nh]={0},lon_moy_temp[nh]={0};
        std::vector<cluster> clust(n_traj);
        std::chrono::time_point<std::chrono::system_clock>start,end;
        ////////    initialization  clusters   ///////
        for(int i=0  ; i<n_traj;i++){
            clust[i].trajectories.push_back(i);
            clust[i].lat_moy.resize(nh); //******
            clust[i].lon_moy.resize(nh); //******
            for(int n=0;n<nh;n++){
                clust[i].lat_moy[n]=trajectory[i].lat[n];
                clust[i].lon_moy[n]=trajectory[i].lon[n];
            }
        }
        double la[nh]={0},lo[nh]={0};
        long double csv=0.0,tv=0.0;
        int c1=clust.size()+5,c2=clust.size()+5;
        //TV_th.resize(nth);
        std::vector<std::thread>th;
        start= std::chrono::system_clock::now();
        if(nth >= clust.size()){/////////////////////////
            nth = clust.size()-1;
            TV_th.resize(nth);
            th.resize(nth);
        }else{
            TV_th.resize(nth);
            th.resize(nth);
        }
        for(int j=0;j<nth;j++){ ///////////////////////
            TV_th[j].la.resize(nh);
            TV_th[j].lo.resize(nh);
        }
        /*In order to save the clustering results, Clusters_list and list_files are used to save the cluuers and the listfiles in the Results folder*/
        std::ofstream clusters_list,list_files;
        clusters_list.open("../Results/Cluster/clusters_list_tsv.txt");
        list_files.open("../Results/Cluster/listfiles_cl.txt");
        for (int f= 0; f < listfiles.size(); f++)
        {
            list_files<<listfiles[f]<<std::endl;
        }
        while(clust.size()>1){
            long double tsv=1E+30;
            if(nth>=clust.size()){
                nth = clust.size()-1;
                TV_th.erase(TV_th.end()-1);
                TV_th.shrink_to_fit();
                th.erase(th.end()-1);
                th.shrink_to_fit();
            }
            int s=0;
            int k=0;
            for (int i=0 ;i<nth-1;i++){
                s=(i+1)*int((clust.size()-1)/nth);
                th[i] = std::thread (fun_TSV,clust,k,s,i,tsv,nh,skip);
                //std::cout << "  s   ="<<s<<"   k    "<<k<<std::endl;
                k=s;
            }
            s=clust.size()-1;
            // std::cout << "  s   ="<< s <<"   k    "<< k <<std::endl;
            th[nth-1] =std::thread (fun_TSV,clust,k,s,nth-1,tsv,nh,skip);
            for (int i=0 ;i<nth;i++){
                th[i].join();
            }
            int u=0;
            for (int num_th=0; num_th < nth-1; num_th++){
                if (TV_th[u].tsv>TV_th[num_th+1].tsv){
                    u=num_th+1;
                }
            }
            tsv = TV_th[u].tsv;
            c1=TV_th[u].c1;
            c2=TV_th[u].c2;
            csv=TV_th[u].csv;
            for(int n=0;n<nh;n++){
                la[n] = TV_th[u].la[n];  // la = sum_lat  : pour garder la valeur de sum_lat du cluster correspondant de min(tsv) , on va l'utiliser pour calculer les coordonnees de trajectoire moyen a la prochene fois
                lo[n] = TV_th[u].lo[n];
            }
            for(int n=0;n<clust[c2].trajectories.size();n++){
                clust[c1].trajectories.push_back(clust[c2].trajectories[n]);
            }
            for (int n=0; n<nh;n++){
                clust[c1].lat_moy[n]= la[n];
                clust[c1].lon_moy[n]=lo[n];
            }
            clust.erase(clust.begin()+c2);
            clust.shrink_to_fit();
            clust[c1].csv=csv;
            std::cout<<clust.size()<<std::endl;//"       c2= "<<c2<<"      c1= "<< c1<<std::endl;
            if (clust.size() < 30) {
                clusters.push_back(clust);
                var_tsv.push_back(tsv);
                clusters_list<<"*** "<<clust.size()<<" clusters ; TSV = "<<tsv<<std::endl;
                for(int n_c=0;n_c<clust.size();n_c++){
                    for(int n_t_c=0;n_t_c<clust[n_c].trajectories.size();n_t_c++){
                        clusters_list<<clust[n_c].trajectories[n_t_c]<<" ";
                    }
                    clusters_list<<"; CSV = " << clust[n_c].csv<<std::endl;
                }
            }
        }
        end= std::chrono::system_clock::now();
        long int microseconds =std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
        out<<"temps = "<<microseconds<<"  microseconde"<< std::endl;
        std::cout<<"Complete clustering."<<std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
}

void old_clustering_kmean (int ncl_initial, int nth, int skip, int repetition){
    try
    {
        if (nh == 0){
            std::cout<<"* * * Error * * *"<<std::endl;
            std::cout<<"    That is because of one of these reasons :"<<std::endl;
            std::cout<<"    1 - You don't read trajectories ."<<std::endl;
            std::cout<<"    2 - The trajectories do not have the same number of endpoints."<<std::endl;
            return;
        }
        clusters.resize(0);
        clusters.shrink_to_fit();
        var_tsv.resize(0);
        var_tsv.shrink_to_fit();
        std::chrono::time_point<std::chrono::system_clock>start,end;
        std::vector<cluster> clust(ncl_initial);
        // std::vector<std::vector<cluster>> clusters;
        std::vector<int> Ci;
        std::vector<std::vector<double>> cluster_befor_update_lat(ncl_initial);
        /**step1 :initialize the centroides randomly  **/
        std::cout<<" step1 :initialize the centroides randomly "<<std::endl;
        srand(time(0));
        int r=0;
        while(r<ncl_initial){               // ncl_initial must be < nbr of trajectories
            int test=0;
            int x = rand()%n_traj;
            for (int i=0;i<r;i++){
                if (Ci[i]==x){    //////////???,i?
                    test=1;
                    break;
                }
            }
            if(test==0){
                Ci.push_back(x);
                clust[r].lat_moy.resize(nh);
                clust[r].lon_moy.resize(nh);
                cluster_befor_update_lat[r].resize(nh);
                for(int n=0;n<nh;n++){
                    clust[r].lat_moy[n]=trajectory[x].lat[n];
                    clust[r].lon_moy[n]=trajectory[x].lon[n];
                }
                r++;
            }
        }
        Ci.resize(0);
        Ci.shrink_to_fit();
        /** step 2 starting k-mean clustering  **/
        int mean_cts = 0;
        std::cout<<"  step 2: starting k-mean clustering  "<<std::endl;
        while (mean_cts !=1)
        {
            for(int n=0;n<clust.size();n++){
                clust[n].trajectories.resize(0);
                clust[n].trajectories.shrink_to_fit();
            }
            for(int w=0;w<ncl_initial;w++){
                for (int n = 0; n < nh; n++)
                {
                    cluster_befor_update_lat[w][n]=clust[w].lat_moy[n];
                }
            }
            for (int i=0 ; i<n_traj;i++){
                double l=100000.0;
                int k=0;
                for(int j=0;j<ncl_initial;j++){
                    double d=0.0;
                    for(int m=0;m<nh;m++){
                        d=d+pow(trajectory[i].lat[m]-clust[j].lat_moy[m],2)+pow(trajectory[i].lon[m]-clust[j].lon_moy[m],2);
                    }
                    if(d<l){
                        k=j;
                        l=d;
                    }
                }
                clust[k].trajectories.push_back(trajectory[i].id);
            }
            /** step 3: update the centroides until clust[w].lat_moy[n] will be cst compared to cluster_befor_update_lat[w][n]*/
            for(int n=0 ;n<ncl_initial; n++ ){
                for(int p=0;p<nh;p++){   
                    double x=0.0,y=0.0; 
                    for(int m=0;m<clust[n].trajectories.size();m++){
                        x=x+trajectory[clust[n].trajectories[m]].lat[p];
                        y=y+trajectory[clust[n].trajectories[m]].lon[p];                    
                    }
                    clust[n].lat_moy[p]=x/clust[n].trajectories.size();
                    clust[n].lon_moy[p]=y/clust[n].trajectories.size();
                }
            }
            mean_cts = 1;
            for(int w=0; w<ncl_initial; w++){
                for (int n = 0; n < nh; n++)
                {
                    if (cluster_befor_update_lat[w][n] != clust[w].lat_moy[n])
                    {
                        mean_cts=0;
                        break;
                    }
                }
                if (mean_cts==0){
                    break;
                }
            }
        }
        /***************************************************/
        double la[nh]={0},lo[nh]={0};
        long double csv=0.0;
        int c1=clust.size()+5,c2=clust.size()+5;
        /************** intial csv of clusters*******************/
        
        for (int t = 0; t < clust.size(); t++)
        {
            long double dd=0.0;
            for (int n=0;n<clust[t].trajectories.size();n++){
                for(int m=0;m<nh;m++){
                    dd=dd+pow(trajectory[clust[t].trajectories[n]].lat[m]-clust[t].lat_moy[m],2) + pow(trajectory[clust[t].trajectories[n]].lon[m]-clust[t].lon_moy[m],2);
                }
            }
            clust[t].csv = dd;
        }
        /**********************************************************************///////////
        std::vector<std::thread> th(nth);
        start= std::chrono::system_clock::now();
        if(nth>=clust.size()){/////////////////////////
            nth = clust.size()-1;
            TV_th.resize(nth);
            th.resize(nth);
        }else{
            TV_th.resize(nth);
            th.resize(nth);
        }
        for(int j=0;j<nth;j++){ ///////////////////////
            TV_th[j].la.resize(nh);
            TV_th[j].lo.resize(nh);
        }
        /*In order to save the clustering results, Clusters_list and list_files are used to save the cluuers and the listfiles in the Results folder*/
        std::ofstream clusters_list,list_files;
        std::stringstream stream;
        std::string rep="";
        stream << repetition;
        stream >> rep;
        std::string path = "../Results/Cluster/clusters_list_kmean_" + rep + ".txt";
        std::cout<<  path <<std::endl;
        clusters_list.open(path);
        list_files.open("../Results/Cluster/listfiles_cl.txt");
        for (int f= 0; f < listfiles.size(); f++)
        {
            list_files<<listfiles[f]<<std::endl;
        }
        while(clust.size()>1){
            long double tsv=1E+30;
            if(nth>=clust.size()){
                nth = clust.size()-1;
                TV_th.erase(TV_th.end()-1);
                TV_th.shrink_to_fit();
                th.erase(th.end()-1);
                th.shrink_to_fit();
            }
            int s=0;
            int k=0;
            for (int i=0 ;i<nth-1;i++){
                s=(i+1)*int((clust.size()-1)/nth);
                th[i] = std::thread (fun_TSV,clust,k,s,i,tsv,nh,skip);
                //std::cout << "  s   ="<<s<<"   k    "<<k<<std::endl;
                k=s;
            }
            s=clust.size()-1;
            // std::cout << "  s   ="<< s <<"   k    "<< k <<std::endl;

            th[nth-1] =std::thread (fun_TSV,clust,k,s,nth-1,tsv,nh,skip);
            for (int i=0 ;i<nth;i++){
                th[i].join();
            }
            
            int u=0;
            for (int num_th=0; num_th < nth-1; num_th++){
                if (TV_th[u].tsv>TV_th[num_th+1].tsv){
                    u=num_th+1;
                }
            }
            tsv = TV_th[u].tsv;
            c1=TV_th[u].c1;
            c2=TV_th[u].c2;
            csv=TV_th[u].csv;
            for(int n=0;n<nh;n++){
                la[n] = TV_th[u].la[n];  // la = sum_lat  : pour garder la valeur de sum_lat du cluster correspondant de min(tsv) , on va l'utiliser pour calculer les coordonnees de trajectoire moyen a la prochene fois
                lo[n] = TV_th[u].lo[n];
            }

            for(int n=0;n<clust[c2].trajectories.size();n++){
                clust[c1].trajectories.push_back(clust[c2].trajectories[n]);
            }
            for (int n=0; n<nh;n++){
                clust[c1].lat_moy[n]= la[n];
                clust[c1].lon_moy[n]=lo[n];
            }
            clust.erase(clust.begin()+c2);
            clust.shrink_to_fit();
            clust[c1].csv=csv;
            std::cout<<clust.size()<<std::endl;
            if (clust.size() < 30) {
                clusters.push_back(clust);
                var_tsv.push_back(tsv);
                clusters_list<<"*** "<<clust.size()<<" clusters ; TSV = "<<tsv<<std::endl;
                for(int n_c=0;n_c<clust.size();n_c++){
                    for(int n_t_c=0;n_t_c<clust[n_c].trajectories.size();n_t_c++){
                        clusters_list<<clust[n_c].trajectories[n_t_c]<<" ";
                    }
                    clusters_list<<"; CSV = " << clust[n_c].csv<<std::endl;
                }
            }
        }
        end = std::chrono::system_clock::now();
        long int microseconds =std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
        std::cout<<"*********** e =  "<<e<<std::endl;
        std::cout<<"Complete clustering."<<std::endl;
        
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
}

void clustering_kmean (int ncl_initial, int nth, int skip, int repetition){
    try
    {
        if (nh == 0){
            std::cout<<"* * * Error * * *"<<std::endl;
            std::cout<<"    That is because of one of these reasons :"<<std::endl;
            std::cout<<"    1 - You don't read trajectories ."<<std::endl;
            std::cout<<"    2 - The trajectories do not have the same number of endpoints."<<std::endl;
            return;
        }
        std::ofstream list_dir_clustering;
        list_dir_clustering.open("../Results/Cluster/list_dir_clustering");
        for (int re = 0; re < repetition; re++)
        {
            clusters.resize(0);
            clusters.shrink_to_fit();
            var_tsv.resize(0);
            var_tsv.shrink_to_fit();
            std::chrono::time_point<std::chrono::system_clock>start,end;
            std::vector<cluster> clust(ncl_initial);
            // std::vector<std::vector<cluster>> clusters;
            std::vector<int> Ci;
            std::vector<std::vector<double>> cluster_befor_update_lat(ncl_initial);
            /**step1 :initialize the centroides randomly  **/
            std::cout<<" step1 :initialize the centroides randomly "<<std::endl;
            srand(time(0));
            int r=0;
            while(r<ncl_initial){               // ncl_initial must be < nbr of trajectories
                int test=0;
                int x = rand()%n_traj;
                for (int i=0;i<r;i++){
                    if (Ci[i]==x){    //////////???,i?
                        test=1;
                        break;
                    }
                }
                if(test==0){
                    Ci.push_back(x);
                    clust[r].lat_moy.resize(nh);
                    clust[r].lon_moy.resize(nh);
                    cluster_befor_update_lat[r].resize(nh);
                    for(int n=0;n<nh;n++){
                        clust[r].lat_moy[n]=trajectory[x].lat[n];
                        clust[r].lon_moy[n]=trajectory[x].lon[n];
                    }
                    r++;
                }
            }
            Ci.resize(0);
            Ci.shrink_to_fit();
            /** step 2 starting k-mean clustering  **/
            int mean_cts = 0;
            std::cout<<"  step 2: starting k-mean clustering  "<<std::endl;
            while (mean_cts !=1)
            {
                for(int n=0;n<clust.size();n++){
                    clust[n].trajectories.resize(0);
                    clust[n].trajectories.shrink_to_fit();
                }
                for(int w=0;w<ncl_initial;w++){
                    for (int n = 0; n < nh; n++)
                    {
                        cluster_befor_update_lat[w][n]=clust[w].lat_moy[n];
                    }
                }
                for (int i=0 ; i<n_traj;i++){
                    double l=100000.0;
                    int k=0;
                    for(int j=0;j<ncl_initial;j++){
                        double d=0.0;
                        for(int m=0;m<nh;m++){
                            d=d+pow(trajectory[i].lat[m]-clust[j].lat_moy[m],2)+pow(trajectory[i].lon[m]-clust[j].lon_moy[m],2);
                        }
                        if(d<l){
                            k=j;
                            l=d;
                        }
                    }
                    clust[k].trajectories.push_back(trajectory[i].id);
                }
                /** step 3: update the centroides until clust[w].lat_moy[n] will be cst compared to cluster_befor_update_lat[w][n]*/
                for(int n=0 ;n<ncl_initial; n++ ){
                    for(int p=0;p<nh;p++){   
                        double x=0.0,y=0.0; 
                        for(int m=0;m<clust[n].trajectories.size();m++){
                            x=x+trajectory[clust[n].trajectories[m]].lat[p];
                            y=y+trajectory[clust[n].trajectories[m]].lon[p];                    
                        }
                        clust[n].lat_moy[p]=x/clust[n].trajectories.size();
                        clust[n].lon_moy[p]=y/clust[n].trajectories.size();
                    }
                }
                mean_cts = 1;
                for(int w=0; w<ncl_initial; w++){
                    for (int n = 0; n < nh; n++)
                    {
                        if (cluster_befor_update_lat[w][n] != clust[w].lat_moy[n])
                        {
                            mean_cts=0;
                            break;
                        }
                    }
                    if (mean_cts==0){
                        break;
                    }
                }
            }
            /***************************************************/
            double la[nh]={0},lo[nh]={0};
            long double csv=0.0;
            int c1=clust.size()+5,c2=clust.size()+5;
            /************** intial csv of clusters*******************/
            
            for (int t = 0; t < clust.size(); t++)
            {
                long double dd=0.0;
                for (int n=0;n<clust[t].trajectories.size();n++){
                    for(int m=0;m<nh;m++){
                        dd=dd+pow(trajectory[clust[t].trajectories[n]].lat[m]-clust[t].lat_moy[m],2) + pow(trajectory[clust[t].trajectories[n]].lon[m]-clust[t].lon_moy[m],2);
                    }
                }
                clust[t].csv = dd;
            }
            /**********************************************************************///////////
            std::vector<std::thread> th(nth);
            start= std::chrono::system_clock::now();
            if(nth>=clust.size()){/////////////////////////
                nth = clust.size()-1;
                TV_th.resize(nth);
                th.resize(nth);
            }else{
                TV_th.resize(nth);
                th.resize(nth);
            }
            for(int j=0;j<nth;j++){ ///////////////////////
                TV_th[j].la.resize(nh);
                TV_th[j].lo.resize(nh);
            }
            /*In order to save the clustering results, Clusters_list and list_files are used to save the cluuers and the listfiles in the Results folder*/
            std::ofstream clusters_list,list_files;
            std::stringstream stream1, stream2;
            std::string rep_tot="", rep;
            stream1 << repetition;
            stream2 << re+1;
            stream1 >> rep_tot;
            stream2 >> rep;
            std::string dir_path = "../Results/Cluster/clustering"  + rep +"_"+ rep_tot;
            list_dir_clustering <<dir_path<<std::endl;
            if (check_dir_exist(dir_path.c_str()) == true)  // c_str to convert string to const char*
            {
                delete_dir_content(dir_path.c_str());
            }else{
                create_dir(dir_path.c_str());
            }
            clusters_list.open((dir_path+ "/clusters_list_kmean.txt").c_str());
            list_files.open((dir_path + "/listfiles_cl.txt").c_str());
            for (int f= 0; f < listfiles.size(); f++)
            {
                list_files<<listfiles[f]<<std::endl;
            }
            while(clust.size()>1){
                long double tsv=1E+30;
                if(nth>=clust.size()){
                    nth = clust.size()-1;
                    TV_th.erase(TV_th.end()-1);
                    TV_th.shrink_to_fit();
                    th.erase(th.end()-1);
                    th.shrink_to_fit();
                }
                int s=0;
                int k=0;
                for (int i=0 ;i<nth-1;i++){
                    s=(i+1)*int((clust.size()-1)/nth);
                    th[i] = std::thread (fun_TSV,clust,k,s,i,tsv,nh,skip);
                    //std::cout << "  s   ="<<s<<"   k    "<<k<<std::endl;
                    k=s;
                }
                s=clust.size()-1;
                // std::cout << "  s   ="<< s <<"   k    "<< k <<std::endl;

                th[nth-1] =std::thread (fun_TSV,clust,k,s,nth-1,tsv,nh,skip);
                for (int i=0 ;i<nth;i++){
                    th[i].join();
                }
                
                int u=0;
                for (int num_th=0; num_th < nth-1; num_th++){
                    if (TV_th[u].tsv>TV_th[num_th+1].tsv){
                        u=num_th+1;
                    }
                }
                tsv = TV_th[u].tsv;
                c1=TV_th[u].c1;
                c2=TV_th[u].c2;
                csv=TV_th[u].csv;
                for(int n=0;n<nh;n++){
                    la[n] = TV_th[u].la[n];  // la = sum_lat  : pour garder la valeur de sum_lat du cluster correspondant de min(tsv) , on va l'utiliser pour calculer les coordonnees de trajectoire moyen a la prochene fois
                    lo[n] = TV_th[u].lo[n];
                }

                for(int n=0;n<clust[c2].trajectories.size();n++){
                    clust[c1].trajectories.push_back(clust[c2].trajectories[n]);
                }
                for (int n=0; n<nh;n++){
                    clust[c1].lat_moy[n]= la[n];
                    clust[c1].lon_moy[n]=lo[n];
                }
                clust.erase(clust.begin()+c2);
                clust.shrink_to_fit();
                clust[c1].csv=csv;
                std::cout<<clust.size()<<std::endl;
                if (clust.size() < 30) {
                    clusters.push_back(clust);
                    var_tsv.push_back(tsv);
                    clusters_list<<"*** "<<clust.size()<<" clusters ; TSV = "<<tsv<<std::endl;
                    for(int n_c=0;n_c<clust.size();n_c++){
                        for(int n_t_c=0;n_t_c<clust[n_c].trajectories.size();n_t_c++){
                            clusters_list<<clust[n_c].trajectories[n_t_c]<<" ";
                        }
                        clusters_list<<"; CSV = " << clust[n_c].csv<<std::endl;
                    }
                }
            }
            end = std::chrono::system_clock::now();
            long int microseconds =std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
            std::cout<<"*********** e =  "<<e<<std::endl;
            std::cout<<"Complete clustering."<<std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
}

void variation_TSV(std::string funct, std::string path_dir){
    try
    {
        std::cout<<"* * * * * * * * * * variation_TSV * * * * * * * * * * "<<std::endl;
        std::ofstream out;
        if (funct =="kmean"){
            out.open(path_dir+"/variation_TSV_kmean.csv");
        }
        if (funct =="TSV"){
            out.open("../Results/Cluster/variation_TSV_tsv.csv");
        }
        out<<"n_clusters"<<";"<<"tsv"<<";"<<"var_tsv"<<std::endl;
        std::cout<<"nbr clusters  "<< "  tsv              "<<"variation"<<std::endl;
        int a = clusters.size();
        for (int i =1; i < var_tsv.size(); i++){
            std::cout<<clusters[i].size()<<"              "<<var_tsv[i]<<"         "<<(var_tsv[i]-var_tsv[i-1])/var_tsv[i]<<std::endl;
            out<<clusters[i].size()<<";"<<var_tsv[i]<<";"<<(var_tsv[i]-var_tsv[i-1])/var_tsv[i]<<std::endl;
        }
        out.close();
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    
    std::cout<<"* * * * * * * * * * End variation_TSV * * * * * * * * * * "<<std::endl;
}

void choice_the_number_of_clusters (int ncl, std::string funct, std::string path_dir){
    try
    {
        std::cout<<"choice_the_number_of_clusters "<<std::endl;
        int n_cluster = clusters.size()-ncl;
        std::ofstream meanclster,clust_list;
        if (funct == "TSV"){
            meanclster.open("../Results/Cluster/meanclsters_TSV.csv");
            clust_list.open("../Results/Cluster/clust_list_tsv");
        } else if (funct == "kmean")
        {
            meanclster.open(path_dir+"/meanclsters_Kmean.csv");
            clust_list.open(path_dir+"/clust_list_kmean");
        }
        for(int x=0; x<clusters[n_cluster].size();x++){
            meanclster<<"lat"<<x<<";"<<"lon"<<x<<";";
        }
        meanclster<<std::endl;
        for(int x=0; x<clusters[n_cluster].size();x++){
            meanclster << clusters[n_cluster][x].trajectories.size()/double(n_traj)<<";"<<clusters[n_cluster][x].trajectories.size()/double(n_traj)<<";"; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        }
        meanclster<<std::endl;
        for(int x=0;x<nh;x++){
            for(int y=0;y<clusters[n_cluster].size();y++){
                meanclster<<clusters[n_cluster][y].lat_moy[x]<<";"<<clusters[n_cluster][y].lon_moy[x]<<";";
            }
            meanclster<<std::endl;
        }
        /*Add to each object a path, the cluster to which it belongs*/
        for(int n=0;n<clusters[n_cluster].size(); n++){
            for(int m=0;m<clusters[n_cluster][n].trajectories.size();m++){
                trajectory[clusters[n_cluster][n].trajectories[m]].cluster=n+1;
            }
        }
        for(int n=0;n<clusters[n_cluster].size(); n++){
            clust_list<<"cluster "<<n+1<<":"<<clusters[n_cluster][n].trajectories.size()/double(n_traj)<<std::endl;
            for(int m=0;m<clusters[n_cluster][n].trajectories.size();m++){
                clust_list<<trajectory[clusters[n_cluster][n].trajectories[m]].cluster <<"  "<<clusters[n_cluster][n].trajectories[m]<<"  "<<listfiles[clusters[n_cluster][n].trajectories[m]]<<std::endl;
            }
        }
        std::cout<<" * * * * * * * * * * * * * * test * * * * * * * * * * * * * *"<<std::endl;
        std::cout << "clusters size" <<"         "<< "n"<<"         "<< "n_cluster";
        std::cout <<"        "<<clusters.size()<<"       "<<ncl<<"           "<<n_cluster<<std::endl;
        int sizze=0;
        for(int n=0;n<clusters[n_cluster].size(); n++){
            std::cout<<clusters[n_cluster][n].trajectories.size()<<std::endl;
            sizze = sizze + clusters[n_cluster][n].trajectories.size();
        }
        nbr_clusters = ncl;
        std::cout<<"The total number of trajectories is :  "<<sizze<<std::endl;
        std::cout<<"TSV :  "<<var_tsv[n_cluster]<<std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
}

void data_to_trajstat (){
    try
    {
        std::cout<<"Started data_to_trajstat ..."<<std::endl;
        /* header data to file and colomn names */
        std::ofstream out;
        out.open("../Results/newdataBeta.csv");
        out<<"date"<<","<<"beta"<<std::endl;
        std::string datte="";
        for (int i=0;i<n_traj;i++){
            std::cout<<trajectory[i].date<<"      "<<trajectory[i].ctr<<std::endl;
            if (trajectory[i].ctr==missing_values){
                continue;
            }
            std::stringstream year,month,day;
            std::string str_year,str_month,str_day,dat;
            year<<trajectory[i].year;
            month<<trajectory[i].month;
            day << trajectory[i].day;
            year>>str_year;
            month>>str_month;
            day>>str_day;
            if (trajectory[i].month<10){
                str_month="0"+str_month;
            }
            if (trajectory[i].day<10){
                str_day="0"+str_day;
            }
            dat = str_year+str_month+str_day;
            if (dat == datte){
                continue;
            }else{
                datte = dat;
            }
            out<<dat<<","<<trajectory[i].ctr<<std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    std::cout<<"Complete data_to_trajstat ..."<<std::endl;
}

void extract_file_data(std::string path, double min, double max, std::string header,std::string file){
    try
    {
        std::cout<<"extract_file_data"<<std::endl;
        if (data.size()==0){
            std::cout<<"Sorry! You must first insert the data. "<<std::endl;
            return;
        }
        std::cout<<"data size="<<data.size()<<std::endl;
        for (int a=1;a<nbr_clusters+1;a++){
            std::stringstream sss;
            std::string ss;
            datta x;
            sss << a;
            sss >>ss;
            x.header= header+ss;
            std::cout<<"+++++++++++++++++"<<x.header<<std::endl;
            x.column.resize(data[index_date_begin].column.size());
            for(int q=0;q<x.column.size();q++){
                x.column[q]="0";
            }
            data.push_back(x);
        }
        std::cout<<"data2 size="<<data.size()<<std::endl;
        for (int i = 0; i < data.size(); i++)
        {
            std::cout<<data[i].header<<std::endl;
        }
        
        std::vector<int> data_date_begin,data_date_end;
        for(int i=0;i<data[index_date_begin].column.size();i++){
            data_date_begin.push_back(date_to_num(data[index_date_begin].column[i]));
            data_date_end.push_back(date_to_num(data[index_date_end].column[i]));
        }
        for(int i=0;i<n_traj;i++){
            for(int j = 0;j<data[index_date_begin].column.size();j++){
                if(data_date_begin[j]<=trajectory[i].date_num && data_date_end[j]>trajectory[i].date_num){
                    for(int k=0 ; k<data.size() ; k++){
                        if(clusters_names[trajectory[i].cluster-1] == data_clusters[k].header){
                            std::stringstream s;
                            std::string st;
                            s << stoi(data[k].column[j])+1;
                            s>>st;
                            data[k].column[j]=st;
                            break;
                        }
                    }
                    break;
                }
            }
        }
        std::ofstream out;
        out.open(path+"/"+file+".csv");
        for(int i=0; i<data.size()-1;i++){
            out<<data[i].header<<";";
        }
        out<<data[data.size()-1].header<<std::endl;
        for (int i = 0; i < data[index_date_begin].column.size(); i++)
        {
            if (stod(data[index_concentration].column[i])>=min && stod(data[index_concentration].column[i])<max ) /////////////////////////
            {
                for(int j=0; j<data.size()-1;j++){
                    out<<data[j].column[i]<<";";
                }
                out<<data[data.size()-1].column[i]<<std::endl;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    std::cout<<"extract_file_data finished"<<std::endl;
}

void add_clusters_to_data_file (std::string funct, std::string path_dir){
    try
    {
        if (groupsadded ==true){
            std::cout<<"The Clusters are already added to Data"<<std::endl;
            return;
        }
        std::cout<<"add_clusters_to_data_file has started"<<std::endl;
        if (data.size()==0){
            std::cout<<"Sorry! You must first insert the data. "<<std::endl;
            return;
        }
        data_clusters.resize(0);
        data_clusters.shrink_to_fit();
        for (int a=0;a<nbr_clusters;a++){
            datta x;
            x.header = clusters_names[a]; //// clusters_names
            x.column.resize(data[index_date_begin].column.size());
            for(int q=0;q<x.column.size();q++){
                x.column[q]="0";
            }
            data_clusters.push_back(x);
        }
        std::cout<<"data2 size="<<data.size()<<std::endl;
        for (int i = 0; i < data.size(); i++)
        {
            std::cout<<data[i].header<<std::endl;
        }
        std::vector<int> data_date_begin,data_date_end;
        for(int i=0;i<data[index_date_begin].column.size();i++){
            data_date_begin.push_back(date_to_num(data[index_date_begin].column[i]));
            data_date_end.push_back(date_to_num(data[index_date_end].column[i]));
        }
        for( int i = 0; i < n_traj; i++ ){
            for( int j = 0; j < data[index_date_begin].column.size(); j++ ){
                if( data_date_begin[j] <= trajectory[i].date_num && data_date_end[j] > trajectory[i].date_num ){
                    for( int k = 0; k < data_clusters.size(); k++ ){
                        if( clusters_names[trajectory[i].cluster-1] == data_clusters[k].header ){
                            std::cout<<"PPPPPPPPPPPPPPPPPPPPP      "<<clusters_names[trajectory[i].cluster-1] <<"   "<<data_clusters[k].column[j] <<"   "<< data_clusters[k].column.size()<<"    ";
                            std::stringstream s;
                            std::string st;
                            s << stoi( data_clusters[k].column[j] ) + 1;
                            s >> st;
                            std::cout<<st<<std::endl;
                            data_clusters[k].column[j] = st;
                            break;
                        }
                    }
                    break;
                }
            }
        }
        for (int j = 0; j < data[index_date_begin].column.size(); j++) // This loop aims to delete unuseful data 
        {
            int y = 0;
            for (int i = 0; i < n_traj; i++)
            {
                if(date_to_num(data[index_date_begin].column[j])<=trajectory[i].date_num && date_to_num(data[index_date_end].column[j])>trajectory[i].date_num){
                    y=1;
                    break;
                }
            }
            if(y == 0){
                for (int n = 0; n < data.size(); n++)
                {
                    data[n].column.erase(data[n].column.begin()+j);
                    data[n].column.shrink_to_fit();
                }
                for (int n = 0; n < data_clusters.size(); n++)
                {
                    data_clusters[n].column.erase(data_clusters[n].column.begin()+j);
                    data_clusters[n].column.shrink_to_fit();
                }
                j--;
            }
        }
        std::ofstream out;
        if (funct == "kmean"){
            out.open(path_dir+"/data_clusters_kmean.csv");
        }else if (funct =="TSV")
        {
            out.open("../Results/Cluster/data_clusters_tsv.csv");
        }
        for(int i=0; i<data.size();i++){
            out<<data[i].header<<";";
        }
        for(int i=0; i<data_clusters.size()-1;i++){
            out<<data_clusters[i].header<<";";
        }
        out<<data_clusters[data_clusters.size()-1].header<<std::endl;

        for (int i = 0; i < data[index_date_begin].column.size(); i++)
        {
            for(int j=0; j<data.size();j++){
                out<<data[j].column[i]<<";";
            }
            for(int j=0; j<data_clusters.size()-1;j++){
                out<<data_clusters[j].column[i]<<";";
            }
            out<<data_clusters[data_clusters.size()-1].column[i]<<std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return;
    }
    clusters_names.resize(nbr_clusters);
    for (int i = 0; i < nbr_clusters; i++)
    {
        clusters_names[i]= data_clusters[i].header;
    }

    std::cout<<"add_clusters_to_data_file finished"<<std::endl;
}

std::string add_groups_to_data_file (std::string funct, std::string path_dir){
    try
    {
        if (groupsadded ==true){
            std::cout<<"The Groups are already added to data"<<std::endl;
            return ""; 
        }
        std::cout<<"add_groups_to_data_file has started"<<std::endl;
        if (data.size()==0){
            std::cout<<"Sorry! You must first insert the data. "<<std::endl;
            return "";
        }
        data_groups_rose.header = "Groups";
        data_groups_rose.column.resize(0);
        data_groups_rose.column.shrink_to_fit();
        data_groups_rose.column.resize(data[index_date_begin].column.size());

        std::vector<int> data_date_begin, data_date_end;
        for(int i=0;i<data[index_date_begin].column.size();i++){
            data_date_begin.push_back(date_to_num(data[index_date_begin].column[i]));
            data_date_end.push_back(date_to_num(data[index_date_end].column[i]));
        }
        for(int i=0;i<data[index_date_begin].column.size();i++){
            for(int j=0;j<group_values.size();j++){
                if (j==0){
                    if (stod(data[index_concentration].column[i])<=group_values[j]){
                        data_groups_rose.column[i]=group_names[j];
                        break;
                    }
                }else if(j==group_values.size()-1){
                    if (stod(data[index_concentration].column[i])>group_values[j]){
                        data_groups_rose.column[i]=group_names[j+1];
                        break;
                    }else if (stod(data[index_concentration].column[i])>=group_values[j-1] && stod(data[index_concentration].column[i])<group_values[j])
                    {
                        data_groups_rose.column[i] = group_names[j];
                        break;
                    }
                }else{
                    if(stod(data[index_concentration].column[i])>=group_values[j-1] && stod(data[index_concentration].column[i])<group_values[j]){
                        data_groups_rose.column[i] = group_names[j];
                        break;
                    }
                }
            }
        }
        for (int j = 0; j < data[index_date_begin].column.size(); j++) // This loop aims to delete unuseful data
        {
            int y = 0;
            for (int i = 0; i < n_traj; i++)
            {
                if(date_to_num(data[index_date_begin].column[j])<=trajectory[i].date_num && date_to_num(data[index_date_end].column[j])>trajectory[i].date_num){
                    y=1;
                    break;
                }
            }
            if(y == 0){
                for (int n = 0; n < data.size(); n++)
                {
                    data[n].column.erase(data[n].column.begin()+j);
                    data[n].column.shrink_to_fit();
                }
                for (int n = 0; n < data_clusters.size(); n++)
                {
                    data_clusters[n].column.erase(data_clusters[n].column.begin()+j);
                    data_clusters[n].column.shrink_to_fit();
                }
                j--;
            }
        }
        std::ofstream out;
        std::stringstream streamm;
        std::string ctr_name;
        streamm << data[index_concentration].header;
        streamm >> ctr_name;
        if (funct == "kmean"){
            out.open( path_dir + "/data_groups_"+ctr_name+"_kmean.csv");
        }else if (funct == "TSV"){
            out.open("../Results/Cluster/data_groups_"+ctr_name+"_tsv.csv");
        }
        for(int i=0; i<data.size();i++){
            out<<data[i].header<<";";
        }
        for(int i=0; i<data_clusters.size();i++){
            out<<data_clusters[i].header<<";";
        }
        out<<data_groups_rose.header<<std::endl;

        for (int i = 0; i < data[index_date_begin].column.size(); i++)
        {
            for(int j=0; j<data.size();j++){
                out<<data[j].column[i]<<";";
            }
            for(int j=0; j<data_clusters.size();j++){
                out<<data_clusters[j].column[i]<<";";
            }
            out<<data_groups_rose.column[i]<<std::endl;
        }
        return ctr_name;
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * : "<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return "";
    }
    groupsadded =true;
    index_groups =data.size()-1;
    std::cout<<"extract_file_data finished"<<std::endl;
}

std::string extract_clusters_roseplot_file(std::string funct, std::string path_dir){
    try
    {
        std::cout<<"extract_roseplot_file has started"<<std::endl;
        if (data.size()==0){
            std::cout<<"Sorry! You must first insert the data. "<<std::endl;
            return "";
        }
        for (int j = 0; j < data[index_date_begin].column.size(); j++)
        {
            int y = 0;
            for (int i = 0; i < n_traj; i++)
            {
                if(date_to_num(data[index_date_begin].column[j])<=trajectory[i].date_num && date_to_num(data[index_date_end].column[j])>trajectory[i].date_num){
                    y=1;
                    break;
                }
            }
            if(y == 0){
                for (int n = 0; n < data.size(); n++)
                {
                    data[n].column.erase(data[n].column.begin()+j);
                    data[n].column.shrink_to_fit();
                }
                for (int n = 0; n < data_clusters.size(); n++)
                {
                    data_clusters[n].column.erase(data_clusters[n].column.begin()+j);
                    data_clusters[n].column.shrink_to_fit();
                }
                j--;
            }
        }
        std::ofstream out;
        std::stringstream streamm;
        std::string ctr_name;
        streamm << data[index_concentration].header;
        streamm >> ctr_name;

        if (funct == "kmean"){
            out.open(path_dir + "/data_groups_"+ ctr_name +"_kmean.csv");
        }else if (funct == "TSV"){
            out.open("../Results/Cluster/data_groups_"+ ctr_name +"_tsv.csv");
        }

        for(int i=0; i<data.size();i++){
            out<<data[i].header<<";";
        }
        for(int i=0; i<data_clusters.size();i++){
            out<<data_clusters[i].header<<";";
        }
        out<<data_groups_rose.header<<std::endl;

        for (int i = 0; i < data[index_date_begin].column.size(); i++)
        {
            for(int j=0; j<data.size();j++){
                out<<data[j].column[i]<<";";
            }
            for(int j=0; j<data_clusters.size();j++){
                out<<data_clusters[j].column[i]<<";";
            }
            out<<data_groups_rose.column[i]<<std::endl;
        }

        std::vector<std::vector<int>> rose;
        rose.resize(nbr_clusters);
        for (int i = 0; i < nbr_clusters; i++)
        {
            rose[i].resize(group_names.size());
            rose[i]={0};
        }
        for (int i = 0; i < data[index_date_begin].column.size(); i++)
        {
            for (int j = 0; j < group_names.size(); j++)
            {
                if(data_groups_rose.column[i] == group_names[j]){
                    for (int n = 0; n < nbr_clusters; n++)
                    {
                        rose[n][j]=rose[n][j] + stoi(data_clusters[n].column[i]);
                    }
                    break;
                }
            }
        }
        std::cout<<"** extract_clusters_roseplot_file started **"<<std::endl;
        std::ofstream out_rose;
        if (funct=="kmean"){
            out_rose.open(path_dir + "/roseplot_file_"+ ctr_name +"_kmean.csv");
        }else if (funct=="TSV"){
            out_rose.open("../Results/Cluster/roseplot_"+ ctr_name +"_file_tsv.csv");
        }
        out_rose<<"Clusters";
        for(int i=0; i<group_names.size();i++){
            out_rose<<";"<<group_names[i];
            if (i==group_names.size()-1){
                out_rose<<std::endl;
            }
        }
        for (int a=0;a<nbr_clusters;a++){
            out_rose << clusters_names[a];
            for (int b=0;b<group_names.size();b++){
                out_rose<<";"<<rose[a][b];
            }
            out_rose<<std::endl;
        }
        std::cout<<"** extract_clusters_roseplot_file finished **"<<std::endl;
        return ctr_name;
    }
    catch(const std::exception& e)
    {
        std::cout<<"* * * Error * * * :"<<std::endl;
        std::stringstream s_err;
        std::string error;
        s_err << e.what()<<std::endl;
        s_err >> error;
        std::cout<< error<<std::endl;
        return "";
    }
    std::cout<<"extract_file_data finished"<<std::endl;
}

void clusters_monthly_frequency(std::string funct, std::string path_dir){
    try
    {
        std::cout<<"Clusters Monthly frequency has been started"<<std::endl;
        int c[nbr_clusters][12]={0};
        std::vector<std::string> month =  {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nev", "Dec"};
        std::ofstream out;
        if (funct=="kmean"){
            out.open( path_dir + "/clusters_monthly_frequency_kmean.csv");
        }else if(funct=="TSV"){
            out.open("../Results/Cluster/clusters_monthly_frequency_tsv.csv");
        }
        out<<"month";
        for (int a=0;a<nbr_clusters;a++){
            out<<";"<<clusters_names[a];
        }
        out<<std::endl;
        for (int a=0;a<n_traj;a++){
            c[trajectory[a].cluster-1][trajectory[a].month-1]++;
        }
        for (int a=0;a<12;a++){
            //out<<a+1;                           /*a----> a+1*/
            out<<month[a];
            for (int b=0;b<nbr_clusters;b++){
                out<<";"<<c[b][a];
            }
            out<<std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return;
    }
    
    std::cout<<"Clusters Monthly frequency has been finished"<<std::endl;
}

void open_clustering(std::string func, std::string path_dir){
    try
    {
        std::cout<<"========================+++++++++++++++++++++++++++++++++++++++++"<<std::endl;
        std::cout<<"Start loading clusters"<<std::endl;
        std::cout<<"========================+++++++++++++++++++++++++++++++++++++++++"<<std::endl;
        //clear_trajs();
        clear_clusters();
        std::string line;
        std::ifstream inFile;
        inFile.open(path_dir+"/listfiles_cl.txt");
        std::vector<std::string> listfiles_clusters;
        while (!inFile.eof()){
            getline(inFile, line);
            if(line != ""){
                listfiles_clusters.push_back(line);
            }
        }
        inFile.close();
        
        if(listfiles_clusters.size()!= listfiles.size()){
            std::cout<<"Error occured when Open-AMA try to: Open_clustering "<<std::endl;
            std::cout<<"Clear and re-read the trajectories used in the clustering "<<std::endl;
            return;
        }
        for(int c=0;c < listfiles.size();c++){
            if (listfiles[c]!= listfiles_clusters[c]){
                std::cout<<"Error occured when Open-AMA try to: Open_clustering "<<std::endl;
                std::cout<<"Clear and re-read the trajectories used in the clustering "<<std::endl;
                return;
            }
        }

        listfiles_clusters.resize(0);
        listfiles_clusters.shrink_to_fit();
        std::cout<<"reading listfiles_cl.txt is done"<<std::endl;
        if (func == "tsv")
        {
            inFile.open(path_dir+"/clusters_list_tsv.txt");
        }else if (func == "kmean")
        {
            inFile.open(path_dir+"/clusters_list_kmean.txt");
        }
        while (!inFile.eof()){
            getline(inFile, line);
            std::stringstream ss(line);
            std::string str = "",nclusters="",str2="",str3="", str4="",str5="", TSV="";
            ss >> str;
            if(str == "***"){
                ss>> nclusters >> str2 >> str3 >> str4 >> str5 >> TSV;
                var_tsv.push_back(stold(TSV));
                int n = stoi(nclusters);
                std::vector<cluster> read_clusters(n);
                for(int i=0;i<n;i++){
                    std::string clusters_part, csv_part , clt, csv, line_clusters, sttr;
                    getline(inFile, line_clusters);
                    std::stringstream s_line(line_clusters);
                    std::getline(s_line, clusters_part, ';');
                    std::getline(s_line, sttr, '=');
                    std::getline(s_line, csv_part);
                    std::stringstream sss1(clusters_part);
                    std::stringstream sss2(csv_part);
                    while (sss1 >> clt )
                    {
                        if (clt != "")
                        {
                            read_clusters[i].trajectories.push_back(stoi(clt));
                        }
                    }
                    sss2 >> csv;
                    
                    read_clusters[i].csv = stold(csv);
                }
                clusters.push_back(read_clusters);
            }
            for (int i = 0; i < clusters.size(); i++)
            {
                for (int j = 0; j < clusters[i].size(); j++)
                {
                    clusters[i][j].lat_moy.resize(nh);
                    clusters[i][j].lon_moy.resize(nh);
                    double lat_moy_temp[nh]={0},lon_moy_temp[nh]={0};
                    for (int k = 0; k < nh; k++)
                    {
                        for (int l = 0; l < clusters[i][j].trajectories.size(); l++)
                        {
                            lat_moy_temp[k] = lat_moy_temp[k] + trajectory[clusters[i][j].trajectories[l]].lat[k]/clusters[i][j].trajectories.size();
                            lon_moy_temp[k] = lon_moy_temp[k] + trajectory[clusters[i][j].trajectories[l]].lon[k]/clusters[i][j].trajectories.size();
                        }
                    }
                    for (int m = 0; m < nh; m++)
                    {
                        lat_moy_temp[m] = lat_moy_temp[m];
                        lon_moy_temp[m] = lon_moy_temp[m];
                        clusters[i][j].lat_moy[m] = lat_moy_temp[m];
                        clusters[i][j].lon_moy[m] = lon_moy_temp[m];
                    }
                }
            }
            
        }
        
        std::cout<<"reading clusters_list is done"<<std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return;
    }
    std::cout<<"loading clusters has been finished"<<std::endl;
}

void freq_hysplit(int ncl, std::string path){
    try
    {
        std::cout<<"========================+++++++++++++++++++++++++++++++++++++++++"<<std::endl;
        std::cout<<"freq_hysplit stareted"<<std::endl;
        std::cout<<"========================+++++++++++++++++++++++++++++++++++++++++";
        int c[ncl][12]={0};
        
        // std::string path="../Data/CLUSLIST_4";
        std::string line;
        std::string file;
        std::ifstream inFile;
        std::ofstream out;
        //out.open("../Results/alpha_beta_freq/.csv");
        out.open(path+".csv");
        out<<"month";
        for (int a=1;a<ncl+1;a++){
            out<<";"<<"C"<<a;
        }
        out<<std::endl;
        inFile.open(path);
        //int t=0;
        //while (!inFile.eof())
        std::cout<<"========================+++++++++++++++++++++++++++++++++++++++++";
        for(int t=0;t<n_traj;t++){
            getline(inFile, line);
            std::stringstream ss(line);
            std::string C="", TC="", Y="", M="", D="", H="";
            ss>> C >> TC >> Y >> M>> D>> H;
            std::stringstream sss;
            std::string str,date1;
            if(stoi(Y)<40){
                sss << stoi(Y)+2000;
                sss >> str;
                date1=str+"-"+M+"-"+D+" "+H+":"+"00";
            }
            else{
                sss << stoi(Y)+1900;
                sss >> str;
                date1=str+"-"+M+"-"+D+" "+H+":"+"00";
            }
            for (int a=0;a<n_traj;a++){
                if (trajectory[a].date==date1){
                    trajectory[a].cluster=stoi(C);
                    trajectory[a].year=stoi(Y);
                    trajectory[a].month=stoi(M);
                    trajectory[a].day=stoi(D);
                    // std::cout<<trajectory[a].year<<"       "<<t<<std::endl;
                    break;
                }
            }
            out.close();
        }
        std::cout<<"========================+++++++++++++++++++++++++++++++++++++++++";
        //std::cout<<"================================"<<std::endl;
        for (int a=0;a<n_traj;a++){
            c[trajectory[a].cluster-1][trajectory[a].month-1]++;
            std::cout<<trajectory[a].month<<"    "<<trajectory[a].cluster<<"     "<<a<<"     "<<n_traj<<std::endl;
        }
        std::cout<<"================================"<<std::endl;
        for (int a=0;a<12;a++){
            out<<a;
            for (int b=0;b<ncl;b++){
                out<<";"<<c[b][a];
            }
            out<<std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return;
    }
    
    std::cout<<"freq_hysplit finished"<<std::endl;
}
