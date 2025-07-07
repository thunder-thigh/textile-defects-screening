#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>

namespace fs = std::filesystem;

void printUsage() {
    std::cout << "Usage:\n"
              << "  ./frame_splicer <video_source> <output_dir> <width> <height> <fabric_speed_mmps> <visible_length_mm> <fabric_roll_id>\n\n"
              << "Example:\n"
              << "  ./frame_splicer rtsp://cam_ip frames/ 1280 720 1200 800 ROLL123\n";
}

int main(int argc, char** argv) {
    if (argc != 8) {
        printUsage();
        return 1;
    }

    std::string videoSource = argv[1];
    std::string outputDir = argv[2];
    int width = std::stoi(argv[3]);
    int height = std::stoi(argv[4]);
    double fabricSpeed = std::stod(argv[5]);       // mm/s
    double visibleLength = std::stod(argv[6]);     // mm
    std::string rollID = argv[7];

    // Effective length per capture (80% of frame visible)
    double advanceLength = visibleLength * 0.8;
    double captureIntervalSec = advanceLength / fabricSpeed;

    std::cout << "📐 Fabric speed: " << fabricSpeed << " mm/s\n";
    std::cout << "📷 Visible length: " << visibleLength << " mm\n";
    std::cout << "⏱️ Capture interval: " << captureIntervalSec << " sec (~" << (1.0 / captureIntervalSec) << " FPS)\n";

    fs::path rollDir = fs::path(outputDir) / rollID;
    fs::create_directories(rollDir);

    // Open stream
    cv::VideoCapture cap(videoSource);
    if (!cap.isOpened()) {
        std::cerr << "❌ Could not open video source: " << videoSource << "\n";
        return 1;
    }

    // Set resolution
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    int frameCount = 0;
    cv::Mat frame;

    auto lastCaptureTime = std::chrono::steady_clock::now() - std::chrono::seconds(10);

    while (true) {
        bool success = cap.read(frame);
        if (!success || frame.empty()) {
            std::cerr << "⚠️  Frame read failed. Exiting.\n";
            break;
        }

        auto now = std::chrono::steady_clock::now();
        double elapsedSec = std::chrono::duration<double>(now - lastCaptureTime).count();

        if (elapsedSec >= captureIntervalSec) {
            // Save frame
            std::ostringstream filename;
            filename << rollDir.string() << "/frame_" << std::setw(5) << std::setfill('0') << frameCount << ".png";
            cv::imwrite(filename.str(), frame);
            std::cout << "✅ Captured frame_" << frameCount << " at " << elapsedSec << "s\n";

            frameCount++;
            lastCaptureTime = now;
        }

        // Slight sleep to avoid hammering CPU (stream is real-time anyway)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    std::cout << "✅ Capture complete. Total frames: " << frameCount << "\n";
    return 0;
}
