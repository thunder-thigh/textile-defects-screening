// RUN AS: "./image_capture ROLL20250707 1333 800 0"
// Parameter          Meaning
// ROLL20250707       Roll ID
// 1333               Fabric speed in mm/s (80 m/min)
// 800                Visible length of fabric in mm (central 80% of the frame)
// 0                  Camera ID

#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <chrono>
#include <thread>
#include <ctime>

namespace fs = std::filesystem;

// === Time Utility ===
std::string current_datetime_string() {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(t, "%Y%m%d_%H%M%S");
    return oss.str();
}

// === Capture and Save Image ===
void capture_image(cv::VideoCapture& cap, const std::string& roll_id, const fs::path& save_dir, double location_mm) {
    cv::Mat frame;
    cap >> frame;

    if (frame.empty()) {
        std::cerr << "⚠️  Frame capture failed. Skipping...\n";
        return;
    }

    double location_m = location_mm / 1000.0;

    std::ostringstream filename;
    filename << roll_id << "_"
             << std::fixed << std::setprecision(2) << location_m << "_"
             << current_datetime_string() << ".png";

    fs::path fullpath = save_dir / filename.str();
    cv::imwrite(fullpath.string(), frame);
    std::cout << "✅ Saved: " << fullpath << "\n";
}

int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "Usage:\n";
        std::cerr << "  ./image_capture <ROLL_ID> <FABRIC_SPEED_MMPS> <VISIBLE_LENGTH_MM> <CAMERA_ID>\n";
        std::cerr << "Example:\n";
        std::cerr << "  ./image_capture ROLL567 1333 800 0\n";
        return 1;
    }

    std::string roll_id = argv[1];
    double speed_mmps = std::stod(argv[2]);           // e.g. 1333 mm/s
    double visible_length_mm = std::stod(argv[3]);    // e.g. 800 mm visible in frame
    int camera_id = std::stoi(argv[4]);

    // Derived interval between captures (80% overlap)
    double capture_spacing_mm = visible_length_mm * 0.8;
    double capture_interval_sec = capture_spacing_mm / speed_mmps;

    // Reverted path (as requested)
    fs::path save_dir = "assets/still_images";
    fs::create_directories(save_dir);

    cv::VideoCapture cap(camera_id);
    if (!cap.isOpened()) {
        std::cerr << "❌ Failed to open camera ID " << camera_id << "\n";
        return 1;
    }

    std::cout << "🎬 Starting capture every " << capture_interval_sec << " seconds...\n";

    double location_mm = 0.0;
    int frame_counter = 0;

    while (true) {
        auto start = std::chrono::steady_clock::now();

        capture_image(cap, roll_id, save_dir, location_mm);

        location_mm += capture_spacing_mm;
        frame_counter++;

        auto end = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        double sleep_time = capture_interval_sec - elapsed;

        if (sleep_time > 0)
            std::this_thread::sleep_for(std::chrono::duration<double>(sleep_time));
    }

    return 0;
}
