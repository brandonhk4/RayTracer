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
            cl::Buffer out(
                context,
                flags,
                sizeof(T) * size
            );
            if (flags != CL_MEM_WRITE_ONLY) {
            queue.enqueueWriteBuffer(out, CL_TRUE, 0, sizeof(T) * size, in);
            }
            return out;
        }

        template<typename T>
        cl::Image2D make_image(T in[], int width, int height, cl_mem_flags flags) {
            cl::Image2D out(
                context,
                flags,
                cl::ImageFormat(CL_RGBA, CL_FLOAT),
                width,
                height,
                sizeof(T) * width * 4,
                in
            );
            return out;
        }

        template<typename T>
        cl::Image2DArray make_image(T in[], int width, int height, int depth, cl_mem_flags flags) {
            cl::Image2DArray out(
                context,
                flags,
                cl::ImageFormat(CL_RGBA, CL_FLOAT),
                depth,
                width,
                height,
                0,
                0,
                in
            );
            return out;
        }

        template<typename... Args>
        void run(std::string function, int size, Args... args) {
            cl::compatibility::make_kernel<Args...> kernel(program, function);
            cl::NDRange global(size);
            cout<<"run call\n";
            kernel(cl::EnqueueArgs(queue, global), args...).wait();
        }

        template<typename... Args>
        void run(std::string function, int width, int height, Args... args) {
            cl::compatibility::make_kernel<Args...> kernel(program, function);
            cl::NDRange global(width, height);
            cout<<"run call\n";
            kernel(cl::EnqueueArgs(queue, global), args...).wait();
        }

        template<typename... Args>
        void run(std::string function, int width, int height, int depth, Args... args) {
            cl::compatibility::make_kernel<Args...> kernel(program, function);
            cl::NDRange global(width, height, depth);
            cout<<"run call\n";
            kernel(cl::EnqueueArgs(queue, global), args...).wait();
        }

        template<typename T>
        void read_buffer(cl::Buffer outBuffer, T out[], int size) {
            queue.enqueueReadBuffer(
                outBuffer,
                CL_TRUE,
                0,
                sizeof(T) * size,
                out
            );
        }

        template<typename T>
        void read_image(cl::Image2D outImage, T out[], int width, int height) {
            const cl::array<cl::size_type, 2> origin{0, 0};
            const cl::array<cl::size_type, 2> region{width, height};
            queue.enqueueReadImage(
                outImage,
                CL_TRUE,
                origin,
                region,
                sizeof(T) * width * 4,
                sizeof(T) * width * height * 4,
                out
            );
        }

        template <typename T>
        void read_image(cl::Image2DArray outImage, T out[], int width, int height, int depth) {
            const cl::array<cl::size_type, 3> origin{0, 0, 0};
            const cl::array<cl::size_type, 3> region{width, height, depth};
            cl_int err = queue.enqueueReadImage(
                outImage,
                CL_TRUE,
                origin,
                region,
                0,
                0,
                out
            );
            if (err != CL_SUCCESS) {
                // Handle immediate queuing error (e.g., invalid arguments)
                std::cout << "Error enqueuing read: " << err << '\n';
            }
        }
};

#endif