from PIL import Image
import os
import natsort
import moviepy.video.io.ImageSequenceClip

# Make a movie from a set of picture
def movie(imageFolder, fps, title):
    imageFiles = [os.path.join(imageFolder, img) for img in os.listdir(imageFolder) if img.endswith(".png")]
    imageFiles = natsort.natsorted(imageFiles, reverse=False)
    print(imageFiles)
    clip = moviepy.video.io.ImageSequenceClip.ImageSequenceClip(imageFiles, fps=fps)
    clip.write_videofile(str(title)+'.mp4')


#Make a movie
movie("output/images", int(5), "output/videos/test")
