//
// Created by julien@macmini on 25/03/2018.
//

#ifndef JOUERENSEMBLE_ANDROID_TRACE_H
#define JOUERENSEMBLE_ANDROID_TRACE_H


class Trace {

public:
    static void beginSection(const char *format, ...);
    static void endSection();
    static void initialize();

private:
    static bool is_tracing_supported_;
};

#endif //JOUERENSEMBLE_ANDROID_TRACE_H
