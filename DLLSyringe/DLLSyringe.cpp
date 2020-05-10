#include <iostream>
#include <windows.h>
#include<string>
using namespace std;

int main()
{
    string titolo;
    cout << "Inserisci il titolo esatto (case sensitive) della finestra ";
    cin >> titolo;
    LPCSTR finestra = titolo.c_str(); //converte la stringa a LPCSTR
    HWND hwnd = FindWindowA(NULL, finestra); // Handle sulla finestra.

    if (hwnd == NULL) {
        cout << "Non trovo la finestra !" << endl; // finestra non trovata.
        Sleep(5000);
        exit(-1);
    }
    else {
        DWORD procID;
        GetWindowThreadProcessId(hwnd, &procID);
        HANDLE handle = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD, 0, procID); // Accesso al processo.

        if (procID == 0) {
            cout << "PID errato!" << endl;
            Sleep(3000);
            exit(-1);
        }
        else {
            cout << "Processo aperto con successo\n";

        }
        cout << "Ora Iniettiamo una DLL\n";
        char Libreria[255];
        cout << "Inserisci il nome della DLL ";
        cin >> Libreria;
        char DLL[255];
        if (!GetFullPathNameA(Libreria, 255, DLL, NULL))  // Trovo la DLL
        {
            cout << "Non trovo la DLL\n";
            Sleep(5000);
            exit(EXIT_FAILURE);
        }
        cout << DLL;
        LPVOID pDllPath = VirtualAllocEx(handle, 0, strlen(DLL) + 1, MEM_COMMIT, PAGE_READWRITE);  // Alloco memoria nel processo
        if (!pDllPath)
        {
            cout << "Non riesco ad allocare la memoria\n";
            Sleep(5000);
            exit(EXIT_FAILURE);
        }
        if (!WriteProcessMemory(handle, pDllPath, (LPVOID)DLL, strlen(DLL) + 1, 0))  // scrivo la DLL nella memoria del processo
        {
            cout << "Non riesco a scrivere la memoria\n";
            Sleep(5000);
            exit(EXIT_FAILURE);
        }
        HANDLE thread = CreateRemoteThread(handle, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA"), pDllPath, 0, 0); // Creo il thread nel processo remoto
        if (!thread)
        {
            cout << "Non riesco a creare il thread nel processo remoto\n";
            Sleep(5000);
            exit(EXIT_FAILURE);
        }
        WaitForSingleObject(thread, INFINITE);  // Attendo l'apertura del thread
        if (VirtualFreeEx(handle, pDllPath, 0, MEM_RELEASE) == 0)
        {
            cout << "Non riesco a liberare la memoria\n";
            cout << GetLastError();
            Sleep(5000);
            exit(EXIT_FAILURE);
        }
        CloseHandle(thread);  // Chiudo gli handle
        CloseHandle(handle);  // Chiudo gli handle
        cout << "DLL Iniettata correttamente\n";
        Sleep(5000);


        return 0;
    }
}

