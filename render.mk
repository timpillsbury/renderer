# render and upload test images to owncloud

BUILD_NUMBER ?= CURRENT
NUM_THREADS ?= 4
FILES := \
	rendered-$(BUILD_NUMBER)/raycaster_cornell_box.png \
	rendered-$(BUILD_NUMBER)/raytracer_cornell_box.png \
	rendered-$(BUILD_NUMBER)/pathtracer_cornell_box.png \
	rendered-$(BUILD_NUMBER)/radiosity_cornell_box.png

.PHONY: render
render: $(FILES)

rendered-$(BUILD_NUMBER):
	mkdir $@

rendered-$(BUILD_NUMBER)/raycaster_cornell_box.pbm: scenes/cornell_box.blend raycaster | rendered-$(BUILD_NUMBER)
	./raycaster $< 640 > $@

rendered-$(BUILD_NUMBER)/raytracer_cornell_box.pbm: scenes/cornell_box.blend raytracer | rendered-$(BUILD_NUMBER)
	./raytracer $< -t$(NUM_THREADS) > $@

rendered-$(BUILD_NUMBER)/pathtracer_cornell_box.pbm: scenes/cornell_box.blend pathtracer | rendered-$(BUILD_NUMBER)
	./pathtracer $< -t$(NUM_THREADS) -m4 -d2 > $@

rendered-$(BUILD_NUMBER)/radiosity_cornell_box.pbm: scenes/cornell_box.blend pathtracer | rendered-$(BUILD_NUMBER)
	./radiosity $< -t$(NUM_THREADS) > $@

rendered-$(BUILD_NUMBER)/raycaster_cornell_box.png: rendered-$(BUILD_NUMBER)/raycaster_cornell_box.pbm
	convert $< $@

rendered-$(BUILD_NUMBER)/raytracer_cornell_box.png: rendered-$(BUILD_NUMBER)/raytracer_cornell_box.pbm
	convert $< $@

rendered-$(BUILD_NUMBER)/pathtracer_cornell_box.png: rendered-$(BUILD_NUMBER)/pathtracer_cornell_box.pbm
	convert $< $@

rendered-$(BUILD_NUMBER)/radiosity_cornell_box.png: rendered-$(BUILD_NUMBER)/radiosity_cornell_box.pbm
	convert $< $@
