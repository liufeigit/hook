#include <mach-o/dyld.h>
#include <stdio.h>
void listImages(){
    uint32_t i;
    uint32_t ic = _dyld_image_count();
    printf("Got %d images\n", ic);
    for (i = 0; i < ic; ++ i) {
        printf("%d: %p\t%s\t(slide: %p)\n",
               i,
               _dyld_get_image_header(i),
               _dyld_get_image_name(i),
               _dyld_get_image_vmaddr_slide(i));
    }
}
int main() {
    listImages();
    return 0;
}