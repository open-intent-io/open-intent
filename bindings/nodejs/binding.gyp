{
    "targets": [
    {
        "target_name": "open-intent",
            "sources": [
                "src/main.cpp", "src/SerialiazableChatbot.cpp"
             ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ],
            'include_dirs': [
                'include', '../../include', '../../../include', '../../thirdparty/json/src'
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
                '-L$(BINARY_DIR)/src', '-lintent-static', '-lboost_regex', '-lboost_log'
            ]
    }
    ]
}
