#define UNICODE
#include <stdio.h>
#include <windows.h>
#include <stdbool.h>

struct processData {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    bool status;
};

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;


HANDLE g_hInputFile = NULL;

struct processData spawnProcess(LPCTSTR program, LPTSTR args) {
    // additional information
    struct processData pd;
    // set the size of the structures
    ZeroMemory(&pd, sizeof(pd));
    pd.si.cb = sizeof(pd.si);
    pd.si.hStdError = g_hChildStd_OUT_Wr;
    pd.si.hStdOutput = g_hChildStd_OUT_Wr;
    pd.si.hStdInput = g_hChildStd_IN_Rd;
    pd.si.dwFlags |= STARTF_USESTDHANDLES;

    pd.status = CreateProcess(
        program, //null terminated program path
        args, //null terminated program args
        NULL, //can child processes inherit handles (null means no)
        NULL, //can child threads inherit handles (null means no)
        TRUE, //inherit parent handles
        0, //creation flags
        NULL, //use parents environment
        NULL, //use parents working directory
        &pd.si,
        &pd.pi
    );
    
    return pd;
}

void sendInput() {
    DWORD dwRead, dwWritten;
    char buffer[] = "sample.js";
    WriteFile(g_hChildStd_IN_Wr, buffer, sizeof(buffer)-1, &dwWritten, NULL);
}

int main() {
    SECURITY_ATTRIBUTES saAttr; 
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 
    if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
      printf(("StdoutRd CreatePipe"));
    if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) 
      printf(("Stdin CreatePipe")); 
    sendInput();
    struct processData pd = spawnProcess(TEXT("C:\\Program Files\\nodejs\\node.exe"), TEXT("node.exe index.js")); //first argument is expected to be just be executable name
    WaitForSingleObject( pd.pi.hProcess, INFINITE );
    DWORD dwRead, dwWritten; 
    char buffer[10000];
    ReadFile(g_hChildStd_OUT_Rd, buffer, 10000, &dwRead, NULL);
    buffer[dwRead] = 0;
    printf("%s", buffer);
    CloseHandle( pd.pi.hProcess );
    CloseHandle( pd.pi.hThread );
}