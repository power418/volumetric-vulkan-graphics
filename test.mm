/*
 * CARA COMPILE DAN RUN:
 * Jalankan perintah di bawah ini pada terminal:
 * 
 * eval clang++ test.mm $(gnustep-config --objc-flags) $(gnustep-config --base-libs) $(gnustep-config --gui-libs) -o test_app && ./test_app
 *
 */

#import <Cocoa/Cocoa.h>
#import <iostream>
#import <string>

// --- Logger Class Interface ---
@interface ConsoleLog : NSObject
+ (void)info:(NSString *)message;
+ (void)warn:(NSString *)message;
+ (void)error:(NSString *)message;
@end

// --- Logger Class Implementation ---
@implementation ConsoleLog
+ (void)info:(NSString *)message {
    std::cout << "[INFO] " << [message UTF8String] << std::endl;
}

+ (void)warn:(NSString *)message {
    // Menggunakan ANSI Escape Codes untuk warna kuning di terminal
    std::cout << "\033[33m[WARN]\033[0m " << [message UTF8String] << std::endl;
}

+ (void)error:(NSString *)message {
    // Menggunakan ANSI Escape Codes untuk warna merah di terminal
    std::cerr << "\033[31m[ERROR]\033[0m " << [message UTF8String] << std::endl;
}
@end


// --- Objective-C Class Interface untuk App Delegate ---
@interface AppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *_window;
}
@end

// --- Objective-C Class Implementation ---
@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    [ConsoleLog info:@"Aplikasi berjalan! Membuat window Cocoa..."];

    // Tentukan ukuran frame window (X, Y, Width, Height)
    NSRect frame = NSMakeRect(0, 0, 800, 600);
    
    // Set gaya window (Ada judul, tombol close, bisa di-resize, dan minimize)
    NSUInteger styleMask = NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask | NSMiniaturizableWindowMask;
    
    // Inisialisasi Window
    _window = [[NSWindow alloc] initWithContentRect:frame
                                          styleMask:styleMask
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
    
    if (_window) {
        [ConsoleLog info:@"Window berhasil dibuat!"];
    } else {
        [ConsoleLog error:@"Gagal membuat window!"];
    }
    
    // Atur judul dan letakkan window di tengah layar, lalu tampilkan
    [_window setTitle:@"Hello Cocoa di GNUstep / Linux!"];
    [_window center];
    [_window makeKeyAndOrderFront:nil];
    
    [ConsoleLog warn:@"Window mungkin terlihat berbeda dengan macOS asli karena kita menggunakan tema GNUstep."];
}

// Tutup aplikasi otomatis saat window disilang (diclose)
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    [ConsoleLog info:@"Window ditutup. Menghentikan aplikasi..."];
    return YES;
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        [ConsoleLog info:@"Inisialisasi NSApplication..."];
        // Inisialisasi Application (Objek utama dari Cocoa)
        NSApplication *app = [NSApplication sharedApplication];
        
        // Hubungkan delegate ke aplikasi
        AppDelegate *delegate = [[AppDelegate alloc] init];
        [app setDelegate:delegate];
        
        [ConsoleLog info:@"Menjalankan Event Loop NSApplication..."];
        // Jalankan event loop (akan memblokir thread ini sampai aplikasi ditutup)
        [app run];
    }
    return 0;
}