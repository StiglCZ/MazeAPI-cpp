#include "image.hh"
#include "maze.hh"

#include "httplib.hh"

#include <csignal>
#include <string>
#include <vector>

constexpr u16
    default_width = 10,
    default_height = 10,
    min_size = 1,
    max_size = 100,
    max_seedlen = 32;

constexpr i32
    default_scale = 2,
    min_scale = 1,
    max_scale = 50;

const std::string
    width_q = "width",
    height_q = "height",
    seed_q = "seed",
    scale_q = "scale";

i32 insecure_strhash_djb2(std::string str) {
    if(str.size() > max_seedlen)
        str = str.substr(0, max_seedlen);

    i32 hash = 0x1505;
    for(char c : str)
        hash = ((hash << 5) + hash) + c;
    return hash;
}

u16 parse_dimension(std::string str, u16 defaultValue, u16 min, u16 max) {
    int _num = std::stoi(str);
    if(_num >= min && _num <= max)
        return static_cast<u16>(_num);
    return defaultValue;
}

void OnRequest(const httplib::Request& req, httplib::Response& res) {
    i32 seed = 0;
    u16 width = default_width, height = default_height;
    i32 scale = default_scale;

    if(req.has_param(width_q))
        width = parse_dimension(req.get_param_value(width_q), default_width, min_size, max_size);
    if(req.has_param(height_q))
        height = parse_dimension(req.get_param_value(height_q), default_height, min_size, max_size);
    if(req.has_param(seed_q))
        seed = insecure_strhash_djb2(req.get_param_value(seed_q));
    if(req.has_param(scale_q))
        scale = parse_dimension(req.get_param_value(scale_q), default_scale, min_scale, max_scale);

    width = width * 2 + 1;
    height = height * 2 + 1;

    u16 scaled_width = width * scale;
    u16 scaled_height = height * scale;

    const std::vector<std::vector<u8>> maze = CreateMaze(width, height, seed);
    u8* rgb = new u8[scaled_width * scaled_height * 3];
    memset(rgb, 0, scaled_width * scaled_height * 3);

    for(u32 x =0; x < width; x++) {
        for(u32 y =0; y < height; y++) {
            switch(maze[x][y]) {
                case Materials::Space:
                    for(int x2 =0; x2 < scale; x2++)
                        for(int y2 =0; y2 < scale; y2++)
                            rgb[((y * scale + y2) * scaled_width + (x * scale + x2)) * 3 + 1] = 0xff;
                    break;
                case Materials::Wall:
                    for(int x2 =0; x2 < scale; x2++)
                        for(int y2 =0; y2 < scale; y2++)
                            rgb[((y * scale + y2) * scaled_width + (x * scale + x2)) * 3 + 0] = 0xff;
                    break;
                case Materials::Start:
                    for(int x2 =0; x2 < scale; x2++)
                        for(int y2 =0; y2 < scale; y2++)
                            rgb[((y * scale + y2) * scaled_width + (x * scale + x2)) * 3 + 2] = 0xff;
                    break;
                case Materials::Finish:
                    for(int x2 =0; x2 < scale; x2++)
                        for(int y2 =0; y2 < scale; y2++) {
                            rgb[((y * scale + y2) * scaled_width + (x * scale + x2)) * 3 + 1] = 0xff;
                            rgb[((y * scale + y2) * scaled_width + (x * scale + x2)) * 3 + 2] = 0xff;
                        }
                    break;
            }
        }
    }

    u8* imageData;
    size_t imageSize = CreateImage(scaled_width, scaled_height, rgb, imageData);
    delete[] rgb;

    // In case the seed is the same
    res.set_header("Cache-Control", "no-cache, no-store, must-revalidate");
    res.set_header("Pragma", "no-cache");
    res.set_header("Expires", "0");

    res.set_content(std::string((char*)imageData, imageSize), "image/jpeg");
    free(imageData);
}

httplib::Server server;

void signalHandler(int) {
    std::cout << "Stopping on signal!" << std::endl;
    server.stop();
    exit(0);
}

void HttpErrorLogger(const httplib::Error& err, const httplib::Request* req) {
    std::cout << "Http Server Error: " << err << std::endl;
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
    signal(SIGTERM, signalHandler);

    server.Get("/", OnRequest);
    server.set_error_logger(HttpErrorLogger);
    std::cout << "Listening on 0.0.0.0:8080" << std::endl;

    bool serverSuccess = server.listen("0.0.0.0", 8080);
    if(serverSuccess) {
        std::cout << "Listening failed!";
        return 1;
    }

    server.stop();
}
