#import <Foundation/Foundation.h>
#import <iostream>

// Objective-C Class Interface
@interface TestClass : NSObject {
    NSString *_name;
}

@property (nonatomic, strong) NSString *name;

- (instancetype)initWithName:(NSString *)name;
- (void)sayHello;

@end

// Objective-C Class Implementation
@implementation TestClass

@synthesize name = _name;

- (instancetype)initWithName:(NSString *)name {
    self = [super init];
    if (self) {
        _name = name;
    }
    return self;
}

- (void)sayHello {
    // You can mix C++ and Objective-C here!
    std::cout << "Hello from C++! My Objective-C name is " << [_name UTF8String] << std::endl;
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // Instantiate the Objective-C class
        TestClass *testObj = [[TestClass alloc] initWithName:@"GNUstep Developer"];
        [testObj sayHello];
    }
    return 0;
}