# RayTracer

This will be a project that fully implements a Ray Tracer from scratch, following the steps in Peter Shirley's "[Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)" Series, plus some personal additions.
<br>

I will be updating this README as I add more features.<br>

Current Features:<br>
Has a for loop that implements the basic concept of a ray tracer. Plan to eventually move onto GPU for parallelism.<br>
<br>
Camera configs for position, orientation, antialiasing, lens focus blurring<br>
<br>
Materials: lambertian, reflective, fuzzy (fuzzier reflections), dielectric<br>
<br>
Config is customizable through command line arguments. A more in-depth explanation here and in the -h printout will be added later.<br>
<br>
Added a GUI display with SFML! So far I have only confirmed that it works on a Windows machine, but it should work on any machine with a decently up-to-date C++ compiler and Git. Will refactor into a separate class later when I start making proper GUI features.