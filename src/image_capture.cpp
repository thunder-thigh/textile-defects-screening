#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <chrono>
#include <thread>
#include <ctime>

namespace fs = std::filesystem;

// === Time utility ===
std::string current_datetime_string() {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(t, "%Y%m%d_%H%M%S");
    return oss.str();
}

int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "Usage: ./fabric_capture <ROLL_ID> <FABRIC_SPEED_MMPS> <INTERVAL_SECONDS> <CAMERA_ID>\n";
        std::cerr << "Example: ./fabric_capture ROLL567 1333 0.5 0\n";
        return 1;
    }

    std::string roll_id = argv[1];
    double speed_mmps = std::stod(argv[2]);        // mm/s (e.g., 1333)
    double interval_sec = std::stod(argv[3]);      // time between captures (e.g., 0.5)
    int camera_id = std::stoi(argv[4]);            // usually 0 for default camera

    fs::path save_dir = "/assets/still_images";
    fs::create_directories(save_dir);

    // Open camera
    cv::VideoCapture cap(camera_id);
    if (!cap.isOpened()) {
        std::cerr << "❌ Failed to open camera ID " << camera_id << "\n";
        return 1;
    }

    std::cout << "🎬 Capturing images every " << interval_sec << "s at " << speed_mmps << " mm/s\n";

    double location_mm = 0.0;
    int frame_counter = 0;

    while (true) {
        auto start = std::chrono::steady_clock::now();

        cv::Mat frame;
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "⚠️ Frame capture failed. Skipping...\n";
            continue;
        }

        // Convert location from mm → meters
        double location_m = location_mm / 1000.0;
        std::ostringstream filename;
        filename << roll_id << "_"
                 << std::fixed << std::setprecision(2) << location_m << "_"
                 << current_datetime_string() << ".png";

        fs::path fullpath = save_dir / filename.str();
        cv::imwrite(fullpath.string(), frame);
        std::cout << "✅ Saved: " << fullpath << "\n";

        // Update position: distance = speed × time
        location_mm += speed_mmps * interval_sec;

        frame_counter++;

        // Sleep for remainder of interval
        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(end - start).count();
        double sleep_time = interval_sec - elapsed;

        if (sleep_time > 0)
            std::this_thread::sleep_for(std::chrono::duration<double>(sleep_time));
    }

    return 0;
}
