# Allegro Nuklear Visual Style Editor (ANVSE)  
Visually edit Nuklear GUI style files.  

Windows 8.1+ x86-64 and Ubuntu 20.04+ x86-64  
Requires Allegro Allegro V5.2.9.0 and Nuklear V4.12.0 to build.  

## About  
The Nuklear GUI, Visual Styles Editor is a fast and easy way to create or modify style tables for the Nuklear GUI system.  
Style files can be loaded, modified and then saved. Just copy and paste the created style table into your project source code.  
The output file is a correctly formatted Nuklear style.c table.  
The editing interface is quite straight forward. Each NK COLOR style can be adjusted using the colour sliders. Click apply under the adjuster and the NK COLOR is applied to the current GUI.  
You can preview the widget that each colour applies to in the right interface, or browse the default Nuklear demo widgets in the example drop down menu.  
If you don't like the colour for that style item, you can reset the colour back to the default loaded from the style file.  
If you want to start with a blank GUI, you can reset all colours with the top button, or reload an existing style file.  
You can load, and modify an existing style file at any time.  

The background Colour drop down is separate from the other adjusters.Here you can adjust alpha transparency for a single colour or for the entire GUI colours as a group.  
You can adjust and set a solid background colour, or use a choice of 2 background images to test the transparency on a background.  
Use the "Lock All" and "Lock None" to switch between single and group alpha adjustment.  
Select "Lock None" followed by "Reset all colours" to clear all adjustments and start again.  

Hopefully developers that make use of Nuklear GUI can find this useful.  

The visual editor is based upon the Allegro 5 graphics library and Nuklear and is available for Windows and Ubuntu x86-64.Allegro, Nuklear Visual Style Editor (ANVSE).Allegro Visual Style Editor (std) source is licenced under MIT0 and free to modify and use.  

## More information  
I had just began familiarizing my self with Allegro 5 and Nuklear when I made this. I quickly discovered that there wasn't any easy way to edit the 28 style colors in the style.c file and thought 28 x 4(rgba) colors to manually edit was a no go for me lol  
Creating ANVSE turned out to be a good way to become familiar with Nuklear :)  
Some of the code can be improved and I may well do so in the future, but for now the application does what I require.  
I made this for my self but have put it up on GitHub so others can make use of it or even find ideas from the code.  
I used the MIT-0 licence so you can do what you like with it. That being said an attribute is always welcome ;)  

I am looking at implimenting some additions in the future such as more options to test alpha transparency on the backgrounds.  
I am also looking at pulling the current operating system theme colors into a style file. If you have ever tried to do this you will know that it is fraught with inconsistancies and gotchas, so don't hold your breath too long :)  

## Some pics to muse at  
**Adding some green to the defualt style with alpha and background.**
![Adding some green to the defualt style with alpha and background.](/img_preview/NK_NVSE_1.png)
![Adding some green to the defualt style with alpha and background.](/img_preview/NK_NVSE_2.png)
![Adding some green to the defualt style with alpha and background.](/img_preview/NK_NVSE_3.png)

**Default theme higlighting margin issue I am still trying to track dowm. The margin doesn't imact the applications function.**
![Adding some green to the defualt style with alpha and background.](/img_preview/NK_Windows.png)
![Adding some green to the defualt style with alpha and background.](/img_preview/NK_Ubuntu.png)
