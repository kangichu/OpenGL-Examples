#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <iostream>

#ifndef IMAGE_UTIL_HPP__
#define IMAGE_UTIL_HPP__

namespace util {
namespace image {

	// Referenced from http://r3dux.org/2010/11/single-call-opengl-texture-loader-in-devil/

	// Function load a image, turn it into a texture, and return the texture ID as a GLuint for use
	GLuint loadImage(const char* theFileName)
	{
		ILuint imageID;				// Create an image ID as a ULuint

		GLuint textureID;			// Create a texture ID as a GLuint

		ILboolean success;			// Create a flag to keep track of success/failure

		ILenum error;				// Create a flag to keep track of the IL error state

		ilGenImages(1, &imageID); 		// Generate the image ID

		ilBindImage(imageID); 			// Bind the image

		success = ilLoadImage(theFileName); 	// Load the image file

		// If we managed to load the image, then we can start to do things with it...
		if (success)
		{
			// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
			ILinfo ImageInfo;
			iluGetImageInfo(&ImageInfo);
			if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			{
				iluFlipImage();
			}

			// Convert the image into a suitable format to work with
			// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
			success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

			// Quit out if we failed the conversion
			if (!success)
			{
				error = ilGetError();
				std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
				exit(-1);
			}

			// Generate a new texture
			glGenTextures(1, &textureID);

			// Bind the texture to a name
			glBindTexture(GL_TEXTURE_2D, textureID);

			// Set texture clamping method
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			// Set texture interpolation method to use linear interpolation (no MIPMAPS)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            
            
			// Specify the texture specification
			glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
						 0,				// Pyramid level (for mip-mapping) - 0 is the top level
						 ilGetInteger(IL_IMAGE_FORMAT),	// Internal image format
						 ilGetInteger(IL_IMAGE_WIDTH),	// Image width
						 ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
						 0,				// Border width in pixels (can either be 1 or 0)
						 ilGetInteger(IL_IMAGE_FORMAT),	// Image format (i.e. RGB, RGBA, BGR etc.)
						 GL_UNSIGNED_BYTE,		// Image data type
						 ilGetData());			// The actual image data itself
		}
		else // If we failed to open the image file in the first place...
		{
			error = ilGetError();
			std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
			exit(-1);
		}

		ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

		std::cout << "Texture creation successful." << std::endl;

		return textureID; // Return the GLuint to the texture so you can use it!
	}


	GLuint
	loadCubemap(string filebase)
	{
	    glActiveTexture(GL_TEXTURE0);
	    GLuint imageID;
        ilGenImages(1, &imageID);
        ilBindImage(imageID);

        glGenTextures(1, &imageID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, imageID);

	    string suffixes[] = {"right","left","top", "bottom","back","front"};
	    GLuint targets[] = {
                GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	    };
	    bool success;
	    ILenum error;
	    for (int i = 0; i < 6; i++) {
	        string texName = filebase + "_" + suffixes[i] + ".png";

	        success = ilLoadImage(texName.c_str());     // Load the image file
	        printf("LOADED %s\n", texName.c_str());

	        // If we managed to load the image, then we can start to do things with it...
	        if (success)
	        {
	            /*
	            // If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
	            ILinfo ImageInfo;
	            iluGetImageInfo(&ImageInfo);
	            if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	            {
	                iluFlipImage();
	            }
	            */


	            // Convert the image into a suitable format to work with
	            // NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
	            success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

	            // Quit out if we failed the conversion
	            if (!success)
	            {
	                error = ilGetError();
	                std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
	                exit(-1);
	            }

	            glTexImage2D(targets[i],                 // Type of texture
	                         0,             // Pyramid level (for mip-mapping) - 0 is the top level
	                         ilGetInteger(IL_IMAGE_FORMAT),    // Internal image format
	                         ilGetInteger(IL_IMAGE_WIDTH),  // Image width
	                         ilGetInteger(IL_IMAGE_HEIGHT), // Image height
	                         0,             // Border width in pixels (can either be 1 or 0)
	                         ilGetInteger(IL_IMAGE_FORMAT), // Image format (i.e. RGB, RGBA, BGR etc.)
	                         GL_UNSIGNED_BYTE,      // Image data type
	                         ilGetData());          // The actual image data itself
                

	        }
	        else // If we failed to open the image file in the first place...
	        {
	            error = ilGetError();
	            std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
	            exit(-1);
	        }

            ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image

	    }

        // Set texture clamping method
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // Set texture interpolation method to use linear interpolation (no MIPMAPS)
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	    return imageID;
	}

}
}

#endif
