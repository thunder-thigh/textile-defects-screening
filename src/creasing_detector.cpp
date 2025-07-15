#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <unordered_set>

namespace fs = std::filesystem;

const std::string INPUT_DIR = "/usr/share/textile-defects/assets/spliced_frames";
const std::string OUTPUT_DIR = "/usr/share/textile-defects/assets/processed_images";

// Supported image extensions
const std::unordered_set<std::string> IMAGE_EXTS = {".png", ".jpg", ".jpeg", ".bmp"};

bool is_image_file(const fs::path& path) {
    return IMAGE_EXTS.count(path.extension().string()) > 0;
}

void apply_canny(const fs::path& input_path, const fs::path& output_path) {
    cv::Mat img = cv::imread(input_path.string(), cv::IMREAD_GRAYSCALE);
    if (img.empty()) {
        std::cerr << "Failed to read image: " << input_path << "\n";
        return;
    }

    cv::Mat edges;
    cv::Canny(img, edges, 100, 200);  // Adjustable thresholds

    if (!cv::imwrite(output_path.string(), edges)) {
        std::cerr << "Failed to save image: " << output_path << "\n";
    } else {
        std::cout << "Processed: " << input_path.filename() << "\n";
    }
}

int main() {
    std::unordered_set<std::string> processed_files;

    fs::create_directories(INPUT_DIR);
    fs::create_directories(OUTPUT_DIR);

    std::cout << "Scanning initial images in " << INPUT_DIR << "\n";

    // Initial processing of existing images
    for (const auto& entry : fs::directory_iterator(INPUT_DIR)) {
        if (!entry.is_regular_file()) continue;
        const fs::path& file_path = entry.path();
        if (!is_image_file(file_path)) continue;

        std::string filename = file_path.filename().string();
        fs::path output_path = fs::path(OUTPUT_DIR) / filename;

        apply_canny(file_path, output_path);
        processed_files.insert(filename);
    }

    std::cout << "Watching for new images...\n";

    // Continuous watch for new files
    while (true) {
        for (const auto& entry : fs::directory_iterator(INPUT_DIR)) {
            if (!entry.is_regular_file()) continue;
            const fs::path& file_path = entry.path();
            if (!is_image_file(file_path)) continue;

            std::string filename = file_path.filename().string();
            if (processed_files.count(filename)) continue;

            fs::path output_path = fs::path(OUTPUT_DIR) / filename;
            apply_canny(file_path, output_path);
            processed_files.insert(filename);
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}

