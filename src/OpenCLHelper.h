#ifndef OPENCLHELPER_H
#define OPENCLHELPER_H

#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/opencl.hpp>

#include "raytracer.h"
#include <fstream>
#include <string>
#include <filesystem>

class OpenCLHelper {
    private:
        cl::Platform default_platform;
        cl::Device default_device;
        cl::Context context;
        cl::CommandQueue queue;
        cl::Program program;

        void initialize() {
            std::vector<cl::Platform> all_platforms;
            cl::Platform::get(&all_platforms);
            default_platform = all_platforms[0];
            std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

            std::vector<cl::Device> all_devices;
            default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
            if (all_devices.size() == 0) {
                std::cout << " No devices found.\n";
                exit(1);
            }

            default_device = all_devices[0];
            std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";

            context = cl::Context(default_device);

            queue = cl::CommandQueue(context, default_device);
 
            std::string file_name = "src/render.cl";
            std::ifstream file(file_name);
            if (!file.is_open()) {
                std::cout << "Failed to open file\n";
                exit(1);
            }
            const int code_length = std::filesystem::file_size(file_name);
            std::string kernel_code(code_length, ' ');
            file.read(&kernel_code[0], code_length);
            file.close();

            program = cl::Program(context, kernel_code);
            if (program.build({ default_device }) != CL_SUCCESS) {
                std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << '\n';
                exit(1);
            }
        }

    public:
        OpenCLHelper() {
            initialize();
        }

        template<typename T>
        cl::Buffer make_buffer(T in[], int size, cl_mem_flags flags) {
            cl::Buffer out(context, flags, sizeof(T) * size);
            if (flags != CL_MEM_WRITE_ONLY) {
            queue.enqueueWriteBuffer(out, CL_TRUE, 0, sizeof(T) * size, in);
            }
            return out;
        }

        template<typename... Args>
        void run(std::string function, int size, Args... args) {
            cl::compatibility::make_kernel<Args...> kernel(program, "fibonacci");
            cl::NDRange global(size);
            kernel(cl::EnqueueArgs(queue, global), args...).wait();
        }

        template<typename T>
        void read(cl::Buffer outBuffer, T out[], int size) {
            queue.enqueueReadBuffer(outBuffer, CL_TRUE, 0, sizeof(T) * size, out);
        }
};

#endif