{
    'targets':
    [
        {
            'target_name': 'Media',
            'type': 'static_library',
            'dependencies':
            [
                '../Source/GameLib/GameLib.gyp:GameLib',
            ],
            'include_dirs':
            [
                'Scripts',
            ],
            'includes':
            [
                'MediaSource.gypi',
            ],
            'msvs_disabled_warnings':
            [
                4503, # Decorated name length exceeded
                4592, # Symbol will be dynamically initialized
            ],
            'all_dependent_settings':
            {
                'include_dirs':
                [
                    'Scripts',
                ],
            },
        },
    ]
}
