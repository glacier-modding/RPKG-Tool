using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows.Interop;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Threading;

namespace rpkg
{
    // TODO: Kill map editor process when main process ends
    class G3dHwndHost : HwndHost
    {
        private static readonly int WINUSER_GCW_ATOM = -32;

        private Process _proc;


        public G3dHwndHost() : this(64.0, 64.0)
        {
        }

        public G3dHwndHost(double w, double h)
        {
            Width = w;
            Height = h;
        }


        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            

            // Setup start info
            ProcessStartInfo newProcInfo = new ProcessStartInfo("map_editor.exe")
            {
                UseShellExecute = false,
                // TODO: Use the mmf, not cmd-line arguments for passing the parent hwnd
                //Arguments = WinuserGetWindowLongPtr(hwndParent.Handle, WINUSER_GCW_ATOM).ToString(),
                Arguments = hwndParent.Handle.ToString(),
                RedirectStandardOutput = true,
                //CreateNoWindow = true
            };

            // Create a memory-mapped file for the new process
            using (MemoryMappedFile mmf = MemoryMappedFile.CreateNew("rpkg_G3dHwndHost_mmf", 16))
            {
                // Create a mutex for access to the file
                bool mutexCreated;
                Mutex mmfMutex = new Mutex(true, "rpkg_G3dHwndHost_mutex", out mutexCreated);

                // Write to mmf
                using (MemoryMappedViewStream oStream = mmf.CreateViewStream())
                {
                    // Create writer
                    BinaryWriter writer = new BinaryWriter(oStream);
                    // Boolean that will become true when the g3d process has handled the file
                    // NOTE: BinaryWriter.Write technically can take an actual boolean, but I'm using this still
                    //  just so it's easier to write to in c++
                    writer.Write((byte)0);
                    // Stringified handle to the parent window
                    // TODO: Use a uint64?
                    writer.Write(hwndParent.Handle.ToString());
                }

                // Release the mutex so the g3d process can use it
                mmfMutex.ReleaseMutex();

                // Start new process
                _proc = Process.Start(newProcInfo);
                _proc.WaitForInputIdle();


                // Get the child hwnd
                UInt64 childHwndInt = 0;
                do
                {
                    // Wait for the access to the mutex
                    mmfMutex.WaitOne();

                    // Read data from the mmf
                    using (MemoryMappedViewStream iStream = mmf.CreateViewStream())
                    {
                        // Create reader
                        BinaryReader reader = new BinaryReader(iStream);
                        // If the g3d process handled the mmf, get the stringified handle to the child window
                        if (reader.ReadByte() > 0)
                            childHwndInt = reader.ReadUInt64();
                    }

                    // Release the mutex, we're done with it
                    mmfMutex.ReleaseMutex();

                } while (childHwndInt == 0);


                // Get handle of the new process
                // TODO: Use a more meaningful variable name
                IntPtr newProcPtr = (IntPtr)childHwndInt;

                // TODO: Handle invalid pointer more elegantly
                if (newProcPtr == null || newProcPtr == IntPtr.Zero)
                    throw new Exception("Null process pointer");

                //WinuserSetParent(newProcPtr, hwndParent.Handle);

                // Update window
                UpdateWindowPos();

                // Return handle to created process
                return new HandleRef(this, newProcPtr);
            }
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            // TODO: I don't think this really works
            _proc.Kill();
        }


        [DllImport("user32.dll", SetLastError = true, EntryPoint = "GetWindowLongPtr")]
        private static extern IntPtr WinuserGetWindowLongPtr(IntPtr hwnd, int nIndex);

        [DllImport("user32.dll", SetLastError = true, EntryPoint = "SetWindowLongPtr")]
        private static extern IntPtr WinuserSetWindowLongPtr(IntPtr hwnd, int nIndex, IntPtr dwNewLong);

        [DllImport("user32.dll", SetLastError = true, EntryPoint = "SetParent")]
        private static extern IntPtr WinuserSetParent(IntPtr child, IntPtr parent);
    }
}
