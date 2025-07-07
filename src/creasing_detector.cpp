#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include <string>
#include <chrono>
#include <thread>

namespace fs = std::filesystem;

void printUsage() {
    std::cout << "Usage:\n"
              << "  ./frame_splicer <video_source> <output_dir> <width> <height> <fps> <fabric_roll_id>\n\n"
              << "Example:\n"
              << "  ./frame_splicer rtsp://camera_ip/live frames/ 1280 720 30 ROLL123\n";
}

int main(int argc, char** argv) {
    if (argc != 7) {
        printUsage();
        return 1;
    }

    std::string videoSource = argv[1];
    std::string outputDir = argv[2];
    int width = std::stoi(argv[3]);
    int height = std::stoi(argv[4]);
    int fps = std::stoi(argv[5]);
    std::string rollID = argv[6];

    // Setup output path
    fs::path rollDir = fs::path(outputDir) / rollID;
    fs::create_directories(rollDir);

    // Open video source
    cv::VideoCapture cap(videoSource);
    if (!cap.isOpened()) {
        std::cerr << "❌ Error: Could not open video source: " << videoSource << "\n";
        return 1;
    }

    // Optional: set resolution
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    int frameCount = 0;
    cv::Mat frame;

    std::cout << "🎥 Starting capture for roll " << rollID << "\n";

    const int delayMs = static_cast<int>(1000.0 / fps);

    while (true) {
        bool success = cap.read(frame);
        if (!success || frame.empty()) {
            std::cerr << "⚠️  Warning: Failed to read frame. Exiting...\n";
            break;
        }

        // Save frame
        std::ostringstream filename;
        filename << rollDir.string() << "/frame_" << std::setw(5) << std::setfill('0') << frameCount << ".png";
        cv::imwrite(filename.str(), frame);

        std::cout << "✅ Saved: " << filename.str() << "\n";

        frameCount++;

        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }

    std::cout << "✅ Capture complete. Total frames saved: " << frameCount << "\n";
    return 0;
}
