#pragma once

#ifdef BUILD_STANDALONE
    // In Standalone mode, a Chapter IS the Application.
    #include "Application.hpp"
    using ChapterBase = Base::Application;
#else
    // In Bundled mode, a Chapter is just a module implementing an interface.
    #include "IChapter.hpp"
    using ChapterBase = IChapter;
#endif