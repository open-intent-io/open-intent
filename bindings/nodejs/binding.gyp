{
    "targets": [
    {
        "target_name": "open-intent",
            "sources": [
                "src/main.cpp",
                "src/SerializableChatbot.cpp",
             ],
            'include_dirs': [
                "include", "../../include", "../../thirdparty/json/src", "../../thirdparty/spdlog/include"
            ],
            "cflags": [
                "-std=c++11",
                "-frtti",
                "-fexceptions"
            ],
            'cflags_cc': [
                "-std=c++11",
                "-frtti",
                "-fexceptions"
            ],
            'libraries': [
                '-L$(BINARY_DIR)/src', '-lintent-static'
            ]
    }]
}
