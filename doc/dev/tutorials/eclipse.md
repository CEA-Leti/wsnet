# Using Eclipse IDE for WSNet development

This tutorial aims to ease and improve development productivity by enabling implementation of WSNet within [Eclipse Integrated Development Environment (IDE)][eclipse].

Eclipse IDE has been widely used in several scenarios in the software development industry. It helps developers by providing a set of frameworks and plug-ins and, thus, enabling a more fluid and straightforward  development.

Moreover, it is valid to stress that Eclipse has a large active community and is still growing. Therefore, finding solutions to several issues usually come handy via dedicated [forums][forums] and [stackoverflow][stackoverflow] like websites.

In this document we will provide a comprehensive tutorial for installing Eclipse along with necessary plugins for developing WSNet modules in a Linux environment. **We assume that you have already properly downloaded and installed WSNet** and is willing to increase your productivity by using a more developer friendly IDE. 

**Notice that you're free to use whatever IDE you feel comfortable. This is just a small tutorial to setup the most common used IDE on the WSNet community.**


## Installation

First of all, we need to download and install Eclipse. An easy way to do it is by following installation instructions found in [Eclipse Foundation's][eclipse].

But wait!

Before downloading any of the available packages, we would like to briefly describe which one better suits our needs. Once WSNet is implemented in C/C++, a convenient option would be the Eclipse CDT for C/C++, which is an integrated development environment (IDE) dedicated for C/C++ developers.

However, you might be planing to use Eclipse for developing not only in C. The good news is that Eclipse is fully plug-in based. Therefore, you may prefer to download the standard version and add the necessary plug-in for each of the programming languages you are willing to use. You can check a comparison table [here][eclipse_compare].

Remember that this tutorial is based on a NoMachine environment. Therefore, you need to download the files for the linux distribution. **Hint: Log-on into any NoMachine instance and use Firefox to download Eclipse, in this way you will be automatically redirected to the correct linux version of Eclipse.** 

Moreover, GRENX machines (the ones used in NoMachine) have an old version of Java as default. Thus, we need to change the path on which Eclipse should look for the Java distribution. In order to change the path you should follow this [tutorial][eclipse_ini] and specify the JVM to ```/path/installed/java/jre1.8.0_40/bin/java```.

Considering you have downloaded and correctly installed Eclipse (either the CDT package or standard with CDT plugins) we can proceed with our tutorial.

## Configuration

In your first usage, Eclipse will demand the location of your ```workspace```, i.e., the disk location where your projects will be stored. We recommend you to use the folder where you performed the command ```git clone```.

Therefore, you have to set your ```workspace``` to the ```wsnet``` folder.

Finally, we need to add the perspectives to the Eclipse environment. You can do it by clicking on the icon highlighted in red as detailed in the Figure below. We are interested on the C/C++ perspective. Note that in our example, the C/C++ perspective is the default (showed in purple) and is already there (showed in blue), once we use the Eclipse CDT.

 <img src="img/eclipse_perspective.png" width="400" hspace="300">
 

### Import Project

Now that all is in place, you need to import the WSNet project to your Eclipse workspace. For that, you will need to click on:

<img src="img/eclipse_import.png" width="400" hspace="300">

Let's say you want to import the ```wsnet``` project. You will have to browse and select this folder to be imported. Regarding the toolchain, you can leave it as ```<none>``` for instance. You can now end this procedure by clicking on ```Finish```. Now you can see that your project ```wsnet_other_instance``` can be accessed in the ```Project Explorer``` view.

### Includes

After configuring as indicated until now, you may also find some errors mainly related to the lack of some libraries. This occurs because Eclipse could not find some libraries. Therefore, we need to manually include these libraries inside the project. The following folders need to be added:

```
/usr/include/
/usr/include/libxml2/
/usr/include/glib-2.0/
```

In order to add the include folders you need to right-click on the project folder and then ```Properties```. You will enter the properties view. Under the ```C/C++ General -> Path and Symbols -> GNU C``` view add the folders indicated above. Figure below shows how the window would look like after you add all folders:

<img src="img/eclipse_includes.png" width="400" hspace="300">

At the end of this process we need to update our index. For that, right-click on your WSNet project and select Index->Rebuild.

<img src="img/eclipse_index_rebuild.png" width="400" hspace="300">

After the indexing process is finished, you will be able to fully use Eclipse for WSNet development. One last advice is the use of the Content Assist (Tap ```Ctrl+Space```) to easily have access to the content of your included functions as shown below:

<img src="img/eclipse_code_assist.png" width="400" hspace="300">


## Conclusion

In this document we described a comprehensive tutorial for enabling Eclipse IDE usage for developing WSNet modules. We hope you may now have Eclipse installed and configured.

Notice, again, that you're free to use whatever IDE you feel comfortable. 

Finally, this tutorial is an ongoing project, thus we may modify it as necessary. Any further required information, please refer to the WSNet project.

[eclipse]: http://www.eclipse.org/
[eclipse_compare]: https://eclipse.org/downloads/compare.php
[eclipse_ini]: https://wiki.eclipse.org/Eclipse.ini
[forums]: http://eclipse.org/forums/
[stackoverflow]: http://stackoverflow.com/questions/tagged/eclipse