#include "bgmrpc.h"
#include <QCoreApplication>
#include <bgmrpccommon.h>

int main(int argc, char* argv[]) {
    qSetMessagePattern("%{time process} %{if-debug}[D]%{function}:%{line} - "
                       "%{endif}%{if-warning}[W]%{function}:%{line} - "
                       "%{endif}%{if-critical}[C]%{function}:%{line} - "
                       "%{endif}%{if-fatal}[F]%{function}:%{line} - "
                       "%{endif}%{if-info}[INFO]%{endif}%{message}");
    QCoreApplication a(argc, argv);

    NS_BGMRPC::BGMRPC RPC;

    RPC.start();

    return a.exec();
}
