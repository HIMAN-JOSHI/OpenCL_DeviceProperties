// header files

// standard headers
#include<stdio.h>
#include<stdlib.h>

// OpenCL headers
#include<CL/opencl.h>

int main(void) {

	// function declarations
	void printOpenCLDeviceProperties(void);

	// code
	printOpenCLDeviceProperties();
}

void printOpenCLDeviceProperties() {

	// code
	printf("OpenCL Information : \n");

	printf("=======================\n");

	cl_int result; // cl_int used instead of int because int is platform dependent and cl_int is platform independent.

	// type of platform
	cl_platform_id ocl_platform_id;

	cl_uint dev_count; // platform independent unsigned 32-bit integer.

	// type of device
	cl_device_id* ocl_device_ids;

	char oclPlatformInfo[512];

	// get first platform ID
	/**
	 * cl_int clGetPlatformIDs(cl_uint num_entries, cl_platform_id *platforms, cl_unit *num_platforms)
	 *
	 * num_entries   - The number of cl_platform_id entries that can be added to platforms.
	 * platforms     - Returns a list of OpenCL platforms found. The cl_platform_id values
	 *                 returned in platforms can be used to identify a specific OpenCL platform (like CPU, GPU, Accelerator, etc.)
	 * num_platforms - Returns the number of OpenCL platforms available.
	 *
	 * Returns CL_SUCCESS if the function is executed successfully, else it returns CL_INVALID_VALUE.
	 *
	*/
	result = clGetPlatformIDs(1, // 1 - as here we are interested in only 1 ID.
		&ocl_platform_id, // ocl_platform_id - give the platformID in this variable.
		NULL); // actual number of platforms found is not important here so this parameter is NULL.
	if (result != CL_SUCCESS) {

		printf("clGetPlatformIDs() failed.\n");
		exit(EXIT_FAILURE);
	}

	// get GPU device count
	/* Obtain the list of devices available on a platform
	 * cl_int clGetDeviceIDs(cl_platform_id platform, cl_device_type device_type, cl_uint num_entries, cl_device_id *devices, cl_uint *num_devices)

	   platform    - Refers to the platform ID returned by clGetPlatformIDs or can be NULL.
	   device_type - A bitfield that identifies the type of OpenCL device. It can be used to query specific OpenCL device or all OpenCL devices available. (like - CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU, GL_DEVICE_TYPE_ACCELERATOR, etc.)
	   num_entries - The number of device IDs that can be added to devices.
	   devices     - A list of OpenCL devices found.
	   num_devices - The number of OpenCL devices available that match device_type.
	*/
	result = clGetDeviceIDs(ocl_platform_id, CL_DEVICE_TYPE_GPU, 0, NULL, &dev_count);
	if (result != CL_SUCCESS) {
		printf("clGetDeviceIDs failed.\n");
		exit(EXIT_FAILURE);
	}
	else if (dev_count == 0) {
		printf("There is No OpenCL supported device on this system.\n");
		exit(EXIT_FAILURE);
	}
	else {

		// get platform name (here the platform name refers to the vendor which implemented this OpenCL).
		/*
			Get specific information about the OpenCL platform.

			cl_int clGetPlatformInfo(cl_platform_id platform, cl_platform_info param_name, size_t param_value_size, void *param_value, size_t *param_value_size_ret)

			platform - The platform ID returned by clGetPlatformIDs or can be NULL.

		*/
		clGetPlatformInfo(ocl_platform_id, // which platform ?
			CL_PLATFORM_NAME, // which information we want ? (here we are interested in the platform name)
			500, // (upto) size of value / name
			&oclPlatformInfo, // store info in this variable.
			NULL); // actual length of the info (not needed here so NULL).
		printf("OpenCL supporting GPU platform name: %s\n", oclPlatformInfo);

		// get platform version
		clGetPlatformInfo(ocl_platform_id, CL_PLATFORM_VERSION, 500, &oclPlatformInfo, NULL);
		printf("OpenCL supporting GPU Platform version : %s\n", oclPlatformInfo);

		// print supporting device number
		printf("Total number of OpenCL supporting GPU device(s) on this system: %d\n", dev_count);

		// allocate memory to hold those device ids.
		ocl_device_ids = (cl_device_id*)malloc(sizeof(cl_device_id) * dev_count);

		// get values into allocated buffer
		clGetDeviceIDs(ocl_platform_id,
			CL_DEVICE_TYPE_GPU, // here we are interested only in GPU type.
			dev_count, // number of devices that can be added to the device list.
			ocl_device_ids, // store the device list in this variable.
			NULL);

		char ocl_dev_prop[1024];
		int i;
		for (i = 0; i < (int)dev_count; i++) {
			printf("\n");
			printf("*********** GPU DEVICE GENERAL INFORMATION ***********\n");
			printf("======================================================\n");
			printf("GPU Device Number : %d\n", i);

			// get information about an OpenCL device.
			/*
				cl_int clGetDeviceInfo(cl_device_id device, cl_device_info param_name, size_t param_value_size, void *param_value, size_t *param_value_size_ret)
			*/
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_NAME, sizeof(ocl_dev_prop), &ocl_dev_prop, NULL);
			printf("GPU Device Name: %s\n", ocl_dev_prop);

			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_VENDOR, sizeof(ocl_dev_prop), &ocl_dev_prop, NULL);
			printf("GPU Device Vendor: %s\n", ocl_dev_prop);

			clGetDeviceInfo(ocl_device_ids[i], CL_DRIVER_VERSION, sizeof(ocl_dev_prop), &ocl_dev_prop, NULL);
			printf("GPU Driver Version: %s\n", ocl_dev_prop);

			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_VERSION, sizeof(ocl_dev_prop), &ocl_dev_prop, NULL);
			printf("GPU Device OpenCL version: %s\n", ocl_dev_prop);

			cl_uint clock_frequency;
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_frequency), &clock_frequency, NULL);
			printf("GPU Device Clock Rate: %u\n", clock_frequency);

			printf("\n");
			printf("********** GPU DEVICE MEMORY INFORMATION ***************\n");
			printf("========================================================\n");
			
			cl_ulong mem_size;
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
			printf("GPU Device Global Memory Bytes: %llu\n", (unsigned long long)mem_size);

			cl_device_local_mem_type local_mem_type;
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
			printf("GPU Device Local Memory Bytes: %llu\n", (unsigned long long)mem_size);

			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(mem_size), & mem_size, NULL);
			printf("GPU Device Constant Buffer Size Bytes: %llu\n", (unsigned long long) mem_size);

			printf("\n");
			printf("********** GPU DEVICE COMPUTE INFORMATION ***************\n");
			printf("========================================================\n");

			cl_uint compute_units;
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, NULL);
			printf("GPU Device Number Of Parallel Processor Cores: %u\n", compute_units);

			size_t workgroup_size;
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
			printf("GPU Device Work Group Size: %u\n", (unsigned int) workgroup_size);

			size_t workitem_dims;
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(workitem_dims), &workitem_dims, NULL);
			printf("GPU Device Work Item Dimensions : %u\n", (unsigned int) workitem_dims);

			size_t workitem_size[3];
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
			printf("GPU Device Work Item Sizes : %u%u%u\n",(unsigned int) workitem_size[0], (unsigned int) workitem_size[1], (unsigned int) workitem_size[2]);

		}
		free(ocl_device_ids);
	}
}
