#include "scanner.h"
#include "main.h"

void detector::draw_box(float conf, int left, int top, int right, int bottom, cv::Mat& frame)
{
    cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 0, 255));
    const std::string label = cv::format("%.2f", conf);
    cv::putText(frame, label, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255));
}

std::vector<cv::String> detector::get_outputs_names(const cv::dnn::Net& net)
{
    static std::vector<cv::String> names;
    if (names.empty())
    {
        const std::vector<int> outLayers = net.getUnconnectedOutLayers();
        const std::vector<cv::String> layersNames = net.getLayerNames();
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
            names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}
static bool IsCursorVisible()
{
    CURSORINFO mouseInfo = { sizeof(CURSORINFO) };
    if (GetCursorInfo(&mouseInfo))
    {
        const HCURSOR handle = mouseInfo.hCursor;
        return (reinterpret_cast<intptr_t>(handle) > 50000) && (reinterpret_cast<intptr_t>(handle) < 100000);
    }
    return false;
}

void detector::postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs)
{
    std::vector<int> classes_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (size_t i = 0; i < outs.size(); ++i)
    {
        float* data = reinterpret_cast<float*>(outs[i].data);
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            float objectness = data[4];
            if (objectness < m_confidence) continue;

            const cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            cv::Point class_id_point;
            double max_class_score;
            cv::minMaxLoc(scores, nullptr, &max_class_score, nullptr, &class_id_point);
            
            float confidence = objectness * (float)max_class_score;

            if (confidence > m_confidence)
            {
                const int centerX = static_cast<int>(data[0] * frame.cols);
                const int centerY = static_cast<int>(data[1] * frame.rows);
                const int width = static_cast<int>(data[2] * frame.cols);
                const int height = static_cast<int>(data[3] * frame.rows);
                const int left = centerX - width / 2;
                const int top = centerY - height / 2;

                classes_ids.push_back(class_id_point.x);
                confidences.push_back(confidence);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
    }

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, m_confidence, m_threshold, indices);

    for (size_t i = 0; i < indices.size(); ++i)
    {
        const int idx = indices[i];
        const cv::Rect box = boxes[idx];
        draw_box(confidences[idx], box.x, box.y, box.x + box.width, box.y + box.height, frame);
        var::boxX = box.x;
        var::boxY = box.y;
        var::Height = box.height;
        var::Width = box.width;
        if (!IsCursorVisible())
        {
            aimbot::aim_to(var::boxX, var::boxY, var::Width, var::Height + 10);
        }
    }
}


static clock_t s_fps = 0;

void detector::start(cv::Mat& image)
{
    const clock_t current_ticks = clock();

    cv::Mat blob;
    cv::dnn::blobFromImage(image, blob, 1.0 / 255.0,
        cv::Size(m_activation_range, m_activation_range), cv::Scalar(0, 0, 0), true, false);
    m_net.setInput(blob);
    std::vector<cv::Mat> outs;
    m_net.forward(outs, get_outputs_names(m_net));

    postprocess(image, outs);

    const std::string label = cv::format("FPS: %u [%s]", static_cast<unsigned int>(s_fps), m_backend_name.c_str());
    cv::putText(image, label, cv::Point(0, 15),
        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));

    cv::Mat detected_frame;


    image.convertTo(detected_frame, CV_8U);
    frame = detected_frame;
  cv::imshow("sd�ijpououiopsdffsd", detected_frame);

    // number of processed frames per sec.
    const clock_t delta_ticks = clock() - current_ticks;
    if (delta_ticks > 0)
        s_fps = CLOCKS_PER_SEC / delta_ticks;
}
void detector::setupOptimalBackend()
{
    // Try CUDA first (fastest if available)
    try
    {
        m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        
        // Test if CUDA is actually available by trying a forward pass
        cv::Mat test_blob = cv::dnn::blobFromImage(cv::Mat::zeros(m_activation_range, m_activation_range, CV_8UC3), 1.0 / 255.0);
        m_net.setInput(test_blob);
        std::vector<cv::Mat> test_outs;
        m_net.forward(test_outs, get_outputs_names(m_net));
        
        if (!test_outs.empty())
        {
            m_backend_name = "CUDA";
            std::cout << "[Detector] Using CUDA backend (GPU accelerated)" << std::endl;
            return;
        }
    }
    catch (const cv::Exception& e)
    {
        // CUDA not available or failed, try OpenCL
    }
    catch (...)
    {
        // CUDA not available, try OpenCL
    }

    // Try OpenCL (good performance, widely supported)
    if (cv::ocl::haveOpenCL())
    {
        try
        {
            cv::ocl::setUseOpenCL(true);
            cv::ocl::useOpenCL();
            
            m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
            m_net.setPreferableTarget(cv::dnn::DNN_TARGET_OPENCL);
            
            // Test OpenCL
            cv::Mat test_blob = cv::dnn::blobFromImage(cv::Mat::zeros(m_activation_range, m_activation_range, CV_8UC3), 1.0 / 255.0);
            m_net.setInput(test_blob);
            std::vector<cv::Mat> test_outs;
            m_net.forward(test_outs, get_outputs_names(m_net));
            
            if (!test_outs.empty())
            {
                m_backend_name = "OpenCL";
                std::cout << "[Detector] Using OpenCL backend (GPU accelerated)" << std::endl;
                return;
            }
        }
        catch (const cv::Exception& e)
        {
            cv::ocl::setUseOpenCL(false);
        }
        catch (...)
        {
            cv::ocl::setUseOpenCL(false);
        }
    }

    // Fallback to CPU (always available)
    m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
    m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    m_backend_name = "CPU";
    std::cout << "[Detector] Using CPU backend (fallback mode)" << std::endl;
}

detector::detector(std::string dataset_labels_path, std::string yolo_config_path, std::string yolo_weights_path)
{
    std::string line;
    std::ifstream file_labels(dataset_labels_path);

    m_activation_range = 125;

    while (std::getline(file_labels, line))
        m_classes.push_back(line);

    m_net = cv::dnn::readNetFromDarknet(yolo_config_path, yolo_weights_path);
    if (m_net.empty())
    {
        MessageBoxA(NULL, "Failed to load YOLO model! Check file paths.", "Error", MB_ICONERROR);
        exit(1);
    }
    
    if (var::debug_console)
    {
        std::cout << "[Scanner] Model loaded successfully!" << std::endl;
    }

    m_activation_range = 320;
    setupOptimalBackend();
}