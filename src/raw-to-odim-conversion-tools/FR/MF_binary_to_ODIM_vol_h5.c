/******************************************************************************/
/*This program converts Meteo France binary radar volume data files to ODIM   */
/*HDF5 radar volume data files.                                               */
/******************************************************************************/

/*Program: MF_binary_to_ODIM_vol_h5.c*/
/*Author: Hidde Leijnse (KNMI)*/
/*Date: March 2012*/  
/*Version: March 2012*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "hdf5.h"
#include "hdf5_hl.h"

/******************************************************************************/
/*Definition of standard parameters.                                          */
/******************************************************************************/
#define DEG2RAD  0.017453293
#define STRLEN   256
#define NMFROW   512
#define NMFCOL   512
#define MFXSCALE 1000.0
#define MFYSCALE 1000.0
#define RSCALE   1000.0
#define NBINS    256
#define NRAYS    360
#define NODATA   0.0
#define UNDETECT 255.0
#define ZGAIN    1.0
#define ZOFFSET  -10.5
#define VGAIN    0.5
#define VOFFSET  -59.25


herr_t copy_all(hid_t loc_id, const char *name, const H5L_info_t *info, void *op_data);
herr_t H5LTmake_dataset_zip(hid_t loc_id, char *dset_name, int rank, hsize_t *dims, hid_t tid, void *data);

int main(int argc, char **argv)
{
	hid_t h5_tpl, h5_out;
	FILE *infile_id;
	char object_out[STRLEN], *string, *basename, *radname, **infilenames, **varname;
	double x_double, gain, offset, *dataset_elev, *all_elevations, x, y, ascale;
	int i, j, k, *date, *time, data, N_elevations, N_MF_files, scan, file, *init_scan, int_elev, ia, ir;
	unsigned char *rawdata, *cartdata;
	long x_long;
	hsize_t dims[2];
	
	/*Check number of input arguments.*/
	if (argc < 4)
	{
		fprintf(stderr, "Usage: %s template_file.h5 ODIM_file.h5 MF_binary_input_file1 [MF_binary_input_file2....MF_binary_input_fileN]\n", argv[0]);
		exit(1);
	}
	
	/*Check input files.*/
	string = (char *) malloc(STRLEN * sizeof(char));
	radname = (char *) malloc(STRLEN * sizeof(char));
	infilenames = (char **) malloc((argc - 3) * sizeof (char *));
	dataset_elev = (double *) malloc((argc - 3) * sizeof(double));
	all_elevations = (double *) malloc((argc - 2) * sizeof(double));
	date = (int *) malloc((argc - 3) * sizeof(int));
	time = (int *) malloc((argc - 3) * sizeof(int));
	varname = (char **) malloc((argc - 3) * sizeof(char *));
	N_elevations = 0;
	all_elevations[0] = 999999.9;
	N_MF_files = 0;
	for (i = 3; i < argc; i++)
	{
		if ((infile_id = fopen(argv[i], "rb")) != NULL)
		{
			/*Get information from file name.*/
			if (!(basename = strrchr(argv[i], '/'))) basename = argv[i];
			sscanf(basename, "%[^_]_%08d%04d_%03d.%[^.]", radname, &(date[N_MF_files]), &(time[N_MF_files]), &int_elev, string);
			dataset_elev[N_MF_files] = ((double) int_elev) * 0.1;
			varname[N_MF_files] = malloc(STRLEN * sizeof(char));
			if (strcmp(string, "PLUIE") == 0) sprintf(varname[N_MF_files], "DBZH");
			if (strcmp(string, "VITESSE") == 0) sprintf(varname[N_MF_files], "VRAD");
			
			j = 0;
			while (dataset_elev[N_MF_files] > (1.0001 * all_elevations[j])) j += 1;
			if (fabs(dataset_elev[N_MF_files] - all_elevations[j]) > 0.0001)
			{
				for (k = N_elevations; k >= j; k--) all_elevations[k + 1] = all_elevations[k];
				all_elevations[j] = dataset_elev[N_MF_files];
				N_elevations += 1;
			}
			
			infilenames[N_MF_files] = argv[i];
			fclose(infile_id);
			N_MF_files += 1;
		}
	}
	
	if (N_MF_files < 1)
	{
		fprintf(stderr, "Error: No valid MF binary volume input files could be opened!\n");
		exit(2);
	}
	
	/*Open template file.*/
	if ((h5_tpl = H5Fopen(argv[1], H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
	{
		fprintf(stderr, "Error: HDF5 template file %s could not be opened!\n", argv[1]);
		exit(2);
	}
	
	/*Open output file.*/
	if ((h5_out = H5Fcreate(argv[2], H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0)
	{
		fprintf(stderr, "Error: ODIM HDF5 output file %s could not be opened!\n", argv[2]);
		H5Fclose(h5_tpl);
		exit(3);
	}
	
	/*Copy template file contents to output file.*/
	H5LTget_attribute_string(h5_tpl, "/", "Conventions", string);
	H5LTset_attribute_string(h5_out, "/", "Conventions", string);
	H5Literate_by_name(h5_tpl, "/", H5_INDEX_NAME, H5_ITER_NATIVE, NULL, copy_all, &h5_out, H5P_DEFAULT);
	H5Fclose(h5_tpl);
	
	cartdata = (unsigned char *) malloc(NMFROW * NMFCOL * sizeof(unsigned char));
	rawdata = (unsigned char *) malloc(NRAYS * NBINS * sizeof(unsigned char));
	init_scan = (int *) malloc(N_elevations * sizeof(int));
	for (i = 0; i < N_elevations; i++) init_scan[i] = 0;
	
	/*Loop over all input files.*/
	for (file = 0; file < N_MF_files; file++)
	{
		/*Write time information to top-level what group.*/
		sprintf(string, "%08d", date[file]);
		H5LTset_attribute_string(h5_out, "/what", "date", string);
		sprintf(string, "%06d", time[file]);
		H5LTset_attribute_string(h5_out, "/what", "time", string);
		
		/*Search for scan number.*/
		scan = 0;
		while (fabs(all_elevations[scan] - dataset_elev[file]) > 0.0001) scan += 1;
		
		/*Write information to ODIM HDF5 file.*/
		if (init_scan[scan] == 0)
		{
			/*Create scan group.*/
			sprintf(object_out, "/dataset%d", scan + 1);
			H5Gcreate(h5_out, object_out, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			
			/*Write dataset where attributes.*/
			sprintf(object_out, "/dataset%d/where", scan + 1);
			H5Gcreate(h5_out, object_out, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			H5LTset_attribute_double(h5_out, object_out, "elangle", &(all_elevations[scan]), 1);
			x_double = RSCALE;
			H5LTset_attribute_double(h5_out, object_out, "rscale", &x_double, 1);
			x_double = 0.0;
			H5LTset_attribute_double(h5_out, object_out, "rstart", &x_double, 1);
			x_long = NBINS;
			H5LTset_attribute_long(h5_out, object_out, "nbins", &x_long, 1);
			x_long = NRAYS;
			H5LTset_attribute_long(h5_out, object_out, "nrays", &x_long, 1);
			
			/*Write dataset what attributes.*/
			sprintf(object_out, "/dataset%d/what", scan + 1);
			H5Gcreate(h5_out, object_out, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			H5LTset_attribute_string(h5_out, object_out, "product", "SCAN");
			
			init_scan[scan] = 1;
		}
		
		/*Create data group.*/
		data = 1;
		gain = ZGAIN;
		offset = ZOFFSET;
		if (strcmp(varname[file], "VRAD") == 0)
		{
			data = 2;
			gain = VGAIN;
			offset = VOFFSET;
		}
		sprintf(object_out, "/dataset%d/data%d", scan + 1, data);
		H5Gcreate(h5_out, object_out, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		
		/*Write data what attributes.*/
		sprintf(object_out, "/dataset%d/data%d/what", scan + 1, data);
		H5Gcreate(h5_out, object_out, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5LTset_attribute_string(h5_out, object_out, "quantity", varname[file]);
		H5LTset_attribute_double(h5_out, object_out, "gain", &gain, 1);
		H5LTset_attribute_double(h5_out, object_out, "offset", &offset, 1);
		x_double = NODATA;
		H5LTset_attribute_double(h5_out, object_out, "nodata", &x_double, 1);
		x_double = UNDETECT;
		H5LTset_attribute_double(h5_out, object_out, "undetect", &x_double, 1);
				
		/*Read input file.*/
		infile_id = fopen(infilenames[file], "rb");
		fread(cartdata, sizeof(unsigned char), NMFROW * NMFCOL, infile_id);
		fclose(infile_id);
		
		/*Convert to polar data.*/
		ascale = 360.0 / NRAYS;
		for (ia = 0; ia < NRAYS; ia++)
		{
			for (ir = 0; ir < NBINS; ir++)
			{
				x = (ir + 0.5) * RSCALE * sin((ia + 0.5) * ascale * DEG2RAD); 
				y = -(ir + 0.5) * RSCALE * cos((ia + 0.5) * ascale * DEG2RAD); 
				i = x / MFXSCALE + NMFCOL / 2;
				j = y / MFYSCALE + NMFROW / 2;
				if ((i >= 0) && (i < NMFCOL) && (j >= 0) && (j < NMFROW)) rawdata[ir + ia * NBINS] = cartdata[i + j * NMFCOL];
				else rawdata[ir + ia * NBINS] = (unsigned char) NODATA;
			}
		}
		
		/*Write polar data to file.*/
		sprintf(object_out, "/dataset%d/data%d/data", scan + 1, data);
		dims[0] = NRAYS;
		dims[1] = NBINS;
		if (H5Lexists(h5_out, object_out, H5P_DEFAULT)) H5Ldelete(h5_out, object_out, H5P_DEFAULT);
		H5LTmake_dataset_zip(h5_out, object_out, 2, dims, H5T_NATIVE_UCHAR, rawdata);
		
		free(varname[file]);
	}
	
	/*Close file.*/
	H5Fclose(h5_out);
	
	/*Free memory.*/
	free(string);
	free(radname);
	free(cartdata);
	free(rawdata);
	free(infilenames);
	free(dataset_elev);
	free(all_elevations);
	free(date);
	free(time);
	free(varname);
	free(init_scan);
	
	/*End of program.*/
	return 0;
}

herr_t copy_all(hid_t loc_id, const char *name, const H5L_info_t *info, void *op_data)
{
	hid_t *h5_out;
	
	h5_out = op_data;
	H5Ocopy(loc_id, name, *h5_out, name, H5P_DEFAULT, H5P_DEFAULT);
	
	return 0;
}


/******************************************************************************/
/* Function: H5LTmake_dataset                                                 */
/* Purpose: Creates and writes a dataset of a type tid                        */
/* Return: Success: 0, Failure: -1                                            */
/* Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu                               */
/* Date: March 19, 2001                                                       */
/* Comments:                                                                  */
/* Modifications: Datasets are compressed  (Iwan Holleman, KNMI)              */
/******************************************************************************/

herr_t H5LTmake_dataset_zip(hid_t loc_id, char *dset_name, int rank, hsize_t *dims, hid_t tid, void *data) 
{
	#define H5ZIPLEVEL (6)
	hid_t did, sid, pid;
	
	/*Create the data space for the dataset.*/
	if ((sid = H5Screate_simple(rank, dims, NULL)) < 0) return -1;
	
	/*Create the property list for zipped datasets.*/
	pid = H5Pcreate(H5P_DATASET_CREATE);
	H5Pset_chunk(pid, rank, dims);
	H5Pset_deflate(pid, H5ZIPLEVEL);
	
	/*Create the dataset.*/
	if ((did = H5Dcreate(loc_id, dset_name, tid, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0) goto out;
	
	/*Write the dataset only if there is data to write*/
	if (data)
	{
		if (H5Dwrite(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0) goto out;
	}
	
	/*End access to the dataset and release resources used by it.*/
	if (H5Dclose(did) < 0) return -1;
	
	/*Terminate access to the data space. */
	if (H5Sclose(sid) < 0) return -1;
	
	/*End access to the property list.*/
	if (H5Pclose(pid) < 0) return -1;
	return 0;
	
	out:
		H5Dclose(did);
		H5Sclose(sid);
		H5Sclose(pid);
		return -1;
}

