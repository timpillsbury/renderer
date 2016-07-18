# render and upload test images to owncloud

BUILD_NUMBER ?= CURRENT
NUM_THREADS ?= 4
FILES := \
	rendered/raycaster_cornell_box_$(BUILD_NUMBER).pbm \
	rendered/raytracer_cornell_box_$(BUILD_NUMBER).pbm \
	rendered/pathtracer_cornell_box_$(BUILD_NUMBER).pbm \
	rendered/radiosity_cornell_box_$(BUILD_NUMBER).pbm

.PHONY: render
render: $(FILES)

rendered:
	mkdir rendered

rendered/raycaster_cornell_box_$(BUILD_NUMBER).pbm: scenes/cornell_box.blend raycaster | rendered
	./raycaster $< 640 > $@

rendered/raytracer_cornell_box_$(BUILD_NUMBER).pbm: scenes/cornell_box.blend raytracer | rendered
	./raytracer $< -t$(NUM_THREADS) > $@

rendered/pathtracer_cornell_box_$(BUILD_NUMBER).pbm: scenes/cornell_box.blend pathtracer | rendered
	./pathtracer $< -t$(NUM_THREADS) -m4 -d2 > $@

rendered/radiosity_cornell_box_$(BUILD_NUMBER).pbm: scenes/cornell_box.blend pathtracer | rendered
	./radiosity $< -t$(NUM_THREADS) > $@


OWNCLOUD_URL := http://own.zerovolt.org/remote.php/webdav
OWNCLOUD_USER := renderer

define upload_rule
.PHONY: upload_$(1)
upload_$(1): $(1)
	PBM=$$<; \
	PNG=$$$${PBM%.*}.png; \
	convert $$$$PBM $$$$PNG \
		&& (echo "Uploading $$$$PNG..."; \
			curl -X PUT -u $(OWNCLOUD_USER):$(OWNCLOUD_PASSWORD) "$(OWNCLOUD_URL)/`basename $$$$PNG`" --data-binary @"$$$$PNG") \
		|| (echo "Warning: Could not convert to png. Uploading $$$$PBM..."; \
			curl -X PUT -u $(OWNCLOUD_USER):$(OWNCLOUD_PASSWORD) "$(OWNCLOUD_URL)/`basename $$$$PBM`" --data-binary @"$$$$PBM")

endef


$(foreach file,$(FILES),$(eval $(call upload_rule,$(file))))


.PHONY: upload
upload: $(addprefix upload_,$(FILES))
