#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <thread>

namespace fs = std::filesystem;

// === Time formatter ===
std::string current_datetime_string() {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(t, "%Y%m%d_%H%M%S");
    return oss.str();
}

int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "Usage:\n";
        std::cerr << "  ./feed_splicer <ROLL_ID> <FABRIC_SPEED_MMPS> <VISIBLE_LENGTH_MM> <VIDEO_SOURCE>\n";
        std::cerr << "Example:\n";
        std::cerr << "  ./feed_splicer ROLL789 1333 800 rtsp://camera.local/stream\n";
        return 1;
    }

    std::string roll_id = argv[1];
    double speed_mmps = std::stod(argv[2]);
    double visible_length_mm = std::stod(argv[3]);
    std::string video_source = argv[4];

    // Derived values
    double spacing_mm = visible_length_mm * 0.8;
    double capture_interval_sec = spacing_mm / speed_mmps;

    fs::path output_dir = fs::current_path() / "assets" / "spliced_frames";
    fs::create_directories(output_dir);

    cv::VideoCapture cap;
    if (video_source.find("http") == 0 || video_source.find("rtsp") == 0 || video_source.find("m3u8") != std::string::npos) {
        cap.open(video_source); // network stream
    } else {
        cap.open(video_source); // local file path
    }

    if (!cap.isOpened()) {
        std::cerr << "❌ Failed to open video source: " << video_source << "\n";
        return 1;
    }

    std::cout << "🎞️  Starting feed splice capture...\n";

    double location_mm = 0.0;
    int frame_counter = 0;

    while (true) {
        auto start_time = std::chrono::steady_clock::now();

        cv::Mat frame;
        bool success = cap.read(frame);
        if (!success || frame.empty()) {
            std::cerr << "⚠️  Stream ended or frame empty. Stopping...\n";
            break;
        }

        double location_m = location_mm / 1000.0;

        std::ostringstream filename;
        filename << roll_id << "_"
                 << std::fixed << std::setprecision(2) << location_m << "_"
                 << current_datetime_string() << ".png";

        fs::path fullpath = output_dir / filename.str();
        cv::imwrite(fullpath.string(), frame);
        std::cout << "✅ Saved: " << fullpath << "\n";

        frame_counter++;
        location_mm += spacing_mm;

        // Sleep to match spacing logic
        auto end_time = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(end_time - start_time).count();
        double sleep_duration = capture_interval_sec - elapsed;

        if (sleep_duration > 0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleep_duration));
        }
    }

    cap.release();
    std::cout << "🛑 Capture finished. Total frames saved: " << frame_counter << "\n";

    return 0;
}
