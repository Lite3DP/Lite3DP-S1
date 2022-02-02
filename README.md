# Lite3DP-S1 - A miniature mSLA DLP 3D Printer

Welcome to Lite3DP-S1.

- [Lite3DP-S1 - A miniature mSLA DLP 3D Printer](#lite3dp-s1---a-miniature-msla-dlp-3d-printer)
  - [FAQs](#faqs)
    - [SD Card is not recognized](#sd-card-is-not-recognized)
    - [My printer just prints blobs / My base is a complete blob and failed the rest of the print](#my-printer-just-prints-blobs--my-base-is-a-complete-blob-and-failed-the-rest-of-the-print)
- [Lite3DP-S1 Original Copyright Notice](#lite3dp-s1-original-copyright-notice)

## FAQs

### SD Card is not recognized

Most of the SD cards shipped with the printer were not formatted properly. To fix it, use Windows, macOS, or Linux, to re-format the SD card as `FAT32` and copy your files again.

### My printer just prints blobs / My base is a complete blob and failed the rest of the print

There are two common issues that could be happening:

1. Calibration of your resin (do this first!), as each resin is different and needs a special start/layer time - per resin!

Every resin, even refills, needs to be calibrated on a per printer basis. The Lite3DP is no exception. We are working on a [Calibration Guide](./Calibration%20guide) that explains how to calibrate your particular resin to maximize the results.

2. UV LED is not spreading out.

We have seen a number of the UV LEDs that are "too focused", especially from Mouser.com kits. What this means is that the light shines too brightly, all focused in the middle of the LCD. You can test this by placing a UV reactive piece of paper (most ink-jet printer paper should work) over the LCD to test the LCD pattern. You should see a wide UV light source, filling the paper edge to edge with no harsh focus in the middle.

If you do see a center area highly focused of UV lights, then most likely you need to diffuse the UV LED to get it to spread out. The community has come up with some `patches` to resolve this. See [this reddit post](https://www.reddit.com/r/Lite3DP/comments/sa4k2z/a_makers_effort_to_patch_the_lite3dps1_to_working/) or matching discord threads [here](https://discord.com/channels/898581358258049095/899086165574037514/934479437746020445) and [here](https://discord.com/channels/898581358258049095/898589927049166899/936603955193065473).

# Lite3DP-S1 Original Copyright Notice

Copyright (c) 2020, Lite3DP.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

For additional information, please visit: <https://lite3dp.com/download.html>

Visit for instructions: <https://www.instructables.com/Lite3DP-S1-MSLA-3D-Printer-Arduino-Based-and-Open-/>
