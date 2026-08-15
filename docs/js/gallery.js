(() => {
  // ns-hugo-imp:E:\workspace\notebook\hugo\cpp_notes\themes\hugo-narrow\assets\js\gallery-lightbox.js
  var GalleryLightbox = class {
    constructor(config = {}) {
      this.config = {
        showCaption: config.showCaption !== false
      };
      this.galleries = /* @__PURE__ */ new Map();
      this.currentGalleryId = null;
      this.currentIndex = 0;
      this.isOpen = false;
      this.lastActiveElement = null;
      this.handleKeydown = this.handleKeydown.bind(this);
      this.elements = this.createElements();
    }
    registerGallery(galleryId, items = []) {
      if (!galleryId || !Array.isArray(items) || items.length === 0) {
        return;
      }
      this.galleries.set(galleryId, items);
    }
    open(galleryId, index = 0, options = {}) {
      const items = this.galleries.get(galleryId);
      if (!items || items.length === 0) {
        return;
      }
      this.currentGalleryId = galleryId;
      this.currentIndex = Math.max(0, Math.min(index, items.length - 1));
      this.lastActiveElement = options.triggerElement || document.activeElement;
      if (!this.elements.root.isConnected) {
        document.body.appendChild(this.elements.root);
      }
      this.render();
      this.elements.root.hidden = false;
      this.elements.root.setAttribute("aria-hidden", "false");
      document.documentElement.classList.add("gallery-lightbox-open");
      document.body.classList.add("gallery-lightbox-open");
      document.addEventListener("keydown", this.handleKeydown);
      requestAnimationFrame(() => {
        this.elements.root.classList.add("is-open");
        this.elements.close.focus({ preventScroll: true });
      });
      this.isOpen = true;
    }
    close() {
      if (!this.isOpen) {
        return;
      }
      this.isOpen = false;
      this.elements.root.classList.remove("is-open");
      this.elements.root.hidden = true;
      this.elements.root.setAttribute("aria-hidden", "true");
      this.elements.image.removeAttribute("src");
      this.elements.caption.innerHTML = "";
      document.documentElement.classList.remove("gallery-lightbox-open");
      document.body.classList.remove("gallery-lightbox-open");
      document.removeEventListener("keydown", this.handleKeydown);
      if (this.lastActiveElement && this.lastActiveElement.isConnected && typeof this.lastActiveElement.focus === "function") {
        this.lastActiveElement.focus({ preventScroll: true });
      }
      this.lastActiveElement = null;
    }
    showPrevious() {
      if (this.currentIndex === 0) {
        return;
      }
      this.currentIndex -= 1;
      this.render();
    }
    showNext() {
      const items = this.getCurrentItems();
      if (this.currentIndex >= items.length - 1) {
        return;
      }
      this.currentIndex += 1;
      this.render();
    }
    getCurrentItems() {
      return this.galleries.get(this.currentGalleryId) || [];
    }
    render() {
      const items = this.getCurrentItems();
      const item = items[this.currentIndex];
      if (!item) {
        return;
      }
      const hasCaption = this.config.showCaption && Boolean(item.captionHTML);
      const isSingleItem = items.length <= 1;
      this.elements.image.src = item.src;
      this.elements.image.alt = item.alt || "";
      this.elements.counter.textContent = String(this.currentIndex + 1) + " / " + String(items.length);
      this.elements.caption.innerHTML = hasCaption ? item.captionHTML : "";
      this.elements.overlay.dataset.hasCaption = hasCaption ? "true" : "false";
      this.elements.previous.hidden = isSingleItem;
      this.elements.next.hidden = isSingleItem;
      this.elements.previous.disabled = isSingleItem || this.currentIndex === 0;
      this.elements.next.disabled = isSingleItem || this.currentIndex === items.length - 1;
    }
    createElements() {
      const root = document.createElement("div");
      root.className = "gallery-lightbox";
      root.hidden = true;
      root.setAttribute("aria-hidden", "true");
      root.innerHTML = [
        '<div class="gallery-lightbox__dialog" role="dialog" aria-modal="true" aria-label="Image viewer">',
        '  <button type="button" class="gallery-lightbox__close" aria-label="Close image viewer">&times;</button>',
        '  <button type="button" class="gallery-lightbox__nav gallery-lightbox__nav--prev" aria-label="Previous image">',
        '    <svg viewBox="0 0 24 24" aria-hidden="true" focusable="false">',
        '      <path d="M14.5 5.5L8 12l6.5 6.5" fill="none" stroke="currentColor" stroke-width="2.25" stroke-linecap="round" stroke-linejoin="round" />',
        "    </svg>",
        "  </button>",
        '  <div class="gallery-lightbox__viewport">',
        '    <div class="gallery-lightbox__media">',
        '      <div class="gallery-lightbox__stage">',
        '        <img class="gallery-lightbox__image" alt="" />',
        "      </div>",
        '      <div class="gallery-lightbox__overlay" data-has-caption="false">',
        '        <div class="gallery-lightbox__caption-bubble">',
        '          <div class="gallery-lightbox__caption"></div>',
        "        </div>",
        '        <span class="gallery-lightbox__counter" aria-live="polite"></span>',
        "      </div>",
        "    </div>",
        "  </div>",
        '  <button type="button" class="gallery-lightbox__nav gallery-lightbox__nav--next" aria-label="Next image">',
        '    <svg viewBox="0 0 24 24" aria-hidden="true" focusable="false">',
        '      <path d="M9.5 5.5L16 12l-6.5 6.5" fill="none" stroke="currentColor" stroke-width="2.25" stroke-linecap="round" stroke-linejoin="round" />',
        "    </svg>",
        "  </button>",
        "</div>"
      ].join("");
      const dialog = root.querySelector(".gallery-lightbox__dialog");
      const close = root.querySelector(".gallery-lightbox__close");
      const previous = root.querySelector(".gallery-lightbox__nav--prev");
      const next = root.querySelector(".gallery-lightbox__nav--next");
      dialog.addEventListener("click", (event) => {
        if (!event.target.closest(".gallery-lightbox__media") && !event.target.closest(".gallery-lightbox__nav") && !event.target.closest(".gallery-lightbox__close")) {
          this.close();
        }
      });
      close.addEventListener("click", () => this.close());
      previous.addEventListener("click", () => this.showPrevious());
      next.addEventListener("click", () => this.showNext());
      return {
        root,
        dialog,
        close,
        previous,
        next,
        image: root.querySelector(".gallery-lightbox__image"),
        counter: root.querySelector(".gallery-lightbox__counter"),
        caption: root.querySelector(".gallery-lightbox__caption"),
        overlay: root.querySelector(".gallery-lightbox__overlay")
      };
    }
    handleKeydown(event) {
      if (!this.isOpen) {
        return;
      }
      if (event.defaultPrevented || event.altKey || event.ctrlKey || event.metaKey) {
        return;
      }
      const activeElement = document.activeElement;
      const isTypingTarget = activeElement && (activeElement.tagName === "INPUT" || activeElement.tagName === "TEXTAREA" || activeElement.isContentEditable || activeElement.tagName === "SELECT");
      if (event.key === "Escape") {
        event.preventDefault();
        this.close();
        return;
      }
      if (isTypingTarget) {
        return;
      }
      if (event.key === "ArrowLeft") {
        event.preventDefault();
        this.showPrevious();
      }
      if (event.key === "ArrowRight") {
        event.preventDefault();
        this.showNext();
      }
    }
    destroy() {
      this.close();
      this.galleries.clear();
      if (this.elements.root.isConnected) {
        this.elements.root.remove();
      }
    }
  };
  var gallery_lightbox_default = GalleryLightbox;

  // <stdin>
  function readJSONConfig(id) {
    const configElement = document.getElementById(id);
    if (!configElement || !configElement.textContent) return null;
    try {
      const parsed = JSON.parse(configElement.textContent);
      return typeof parsed === "string" ? JSON.parse(parsed) : parsed;
    } catch (_) {
      return null;
    }
  }
  var SmartGalleryLayoutManager = class {
    constructor(config = {}) {
      this.config = config;
      this.instances = /* @__PURE__ */ new Map();
    }
    initialize(container, items, layout, onItemClick) {
      if (typeof SmartGallery === "undefined") {
        console.error("SmartGallery is not available");
        return null;
      }
      const options = {
        layout: layout || this.config.defaultLayout || this.config.defaultlayout || "justified",
        gap: this.config.gap !== void 0 ? parseInt(this.config.gap, 10) : 10,
        targetRowHeight: this.config.targetRowHeight !== void 0 ? parseInt(this.config.targetRowHeight, 10) : this.config.targetrowheight !== void 0 ? parseInt(this.config.targetrowheight, 10) : 300,
        lastRowBehavior: this.config.lastRowBehavior || this.config.lastrowbehavior || "left",
        columnWidth: this.config.columnWidth !== void 0 ? parseInt(this.config.columnWidth, 10) : this.config.columnwidth !== void 0 ? parseInt(this.config.columnwidth, 10) : 300,
        columns: this.config.columns !== void 0 ? this.config.columns : "auto",
        placeholderColor: "transparent",
        onItemClick: ({ index, originalEvent }) => {
          if (originalEvent && originalEvent.target && originalEvent.target.closest(".layout-btn")) {
            return;
          }
          if (typeof onItemClick === "function") {
            onItemClick(index, originalEvent);
          }
        }
      };
      const gallery = new SmartGallery(container, options);
      gallery.setItems(items);
      this.instances.set(container.id, {
        gallery,
        container,
        items,
        onItemClick
      });
      return gallery;
    }
    switchLayout(containerId, newLayout) {
      const instance = this.instances.get(containerId);
      if (!instance) {
        return;
      }
      instance.gallery.setOptions({ layout: newLayout });
    }
    destroy() {
      this.instances.forEach(({ gallery }) => {
        if (gallery && gallery.destroy) {
          gallery.destroy();
        }
      });
      this.instances.clear();
    }
  };
  var ImageGallery = class {
    constructor() {
      const rawConfig = readJSONConfig("gallery-config") || {};
      const galleryOptions = rawConfig.galleryOptions || {};
      this.config = {
        gallery: rawConfig.gallery,
        lightbox: rawConfig.lightbox,
        galleryOptions
      };
      this.layoutManager = new SmartGalleryLayoutManager(galleryOptions);
      this.lightbox = this.config.lightbox ? new gallery_lightbox_default() : null;
      this.singleImageCount = 0;
      this.init();
    }
    init() {
      if (document.readyState === "loading") {
        document.addEventListener("DOMContentLoaded", () => this.setup(), { once: true });
      } else {
        this.setup();
      }
    }
    setup() {
      this.processImages();
    }
    processImages() {
      const imageFigures = document.querySelectorAll('.image-figure[data-gallery-type="auto"]');
      if (imageFigures.length === 0) {
        return;
      }
      const groups = this.detectImageGroups(imageFigures);
      groups.forEach((group, index) => {
        if (group.length > 1 && this.config.gallery) {
          this.createGalleryGroup(group, index);
        } else {
          this.processIndividualImages(group);
        }
      });
    }
    detectImageGroups(figures) {
      const groups = [];
      let currentGroup = [];
      for (let i = 0; i < figures.length; i += 1) {
        const figure = figures[i];
        const nextFigure = figures[i + 1];
        currentGroup.push(figure);
        if (nextFigure && this.areConsecutiveByEmptyLine(figure, nextFigure)) {
          continue;
        }
        groups.push([...currentGroup]);
        currentGroup = [];
      }
      return groups;
    }
    areConsecutiveByEmptyLine(figure1, figure2) {
      let current = figure1.nextElementSibling;
      while (current && current !== figure2) {
        if (current.nodeType === Node.ELEMENT_NODE) {
          if (current.matches(".image-figure, .gallery-layout-switcher, .smart-gallery-container")) {
            current = current.nextElementSibling;
            continue;
          }
          const tagName = current.tagName.toLowerCase();
          const text = current.textContent.trim();
          if (tagName === "br") {
            current = current.nextElementSibling;
            continue;
          }
          if (tagName === "p" && text === "") {
            return false;
          }
          if (text !== "") {
            return false;
          }
        }
        current = current.nextElementSibling;
      }
      return current === figure2;
    }
    createGalleryGroup(figures, groupIndex) {
      const galleryContainer = document.createElement("div");
      galleryContainer.className = "smart-gallery-container";
      galleryContainer.id = "gallery-" + String(groupIndex);
      const galleryInner = document.createElement("div");
      galleryInner.className = "smart-gallery";
      galleryInner.id = "gallery-inner-" + String(groupIndex);
      galleryInner.dataset.lightboxEnabled = this.lightbox ? "true" : "false";
      const layoutItems = [];
      const lightboxItems = [];
      figures.forEach((figure) => {
        const img = figure.querySelector("img");
        const caption = figure.querySelector(".image-caption");
        if (!img) {
          return;
        }
        const fullSizeSrc = figure.getAttribute("data-image-src") || img.currentSrc || img.src;
        const previewSrc = img.currentSrc || img.src;
        const width = parseInt(figure.getAttribute("data-image-width"), 10) || img.naturalWidth || 800;
        const height = parseInt(figure.getAttribute("data-image-height"), 10) || img.naturalHeight || 600;
        layoutItems.push({
          src: previewSrc,
          width,
          height,
          aspectRatio: width / height
        });
        lightboxItems.push({
          src: fullSizeSrc,
          width,
          height,
          alt: img.alt || "",
          captionHTML: caption ? caption.innerHTML.trim() : ""
        });
      });
      if (layoutItems.length === 0) {
        return;
      }
      const defaultLayout = this.config.galleryOptions.defaultLayout || this.config.galleryOptions.defaultlayout || "justified";
      const switcher = this.createLayoutSwitcher(defaultLayout, (newLayout) => {
        this.layoutManager.switchLayout(galleryInner.id, newLayout);
      });
      galleryContainer.appendChild(switcher);
      galleryContainer.appendChild(galleryInner);
      const firstFigure = figures[0];
      firstFigure.parentNode.insertBefore(galleryContainer, firstFigure);
      figures.forEach((figure) => figure.remove());
      if (this.lightbox) {
        this.lightbox.registerGallery(galleryContainer.id, lightboxItems);
      }
      this.layoutManager.initialize(galleryInner, layoutItems, defaultLayout, (index, originalEvent) => {
        if (this.lightbox) {
          const triggerElement = originalEvent && originalEvent.target ? originalEvent.target.closest(".sg-item") : null;
          this.lightbox.open(galleryContainer.id, index, { triggerElement });
        }
      });
    }
    createLayoutSwitcher(defaultLayout, onSwitch) {
      const switcher = document.createElement("div");
      switcher.className = "gallery-layout-switcher";
      switcher.innerHTML = [
        '<button class="layout-btn" data-layout="justified" title="Justified Layout" aria-label="Switch to justified gallery layout">',
        '  <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">',
        '    <rect x="3" y="4" width="18" height="5" rx="1"/>',
        '    <rect x="3" y="11" width="8" height="9" rx="1"/>',
        '    <rect x="13" y="11" width="8" height="9" rx="1"/>',
        "  </svg>",
        "</button>",
        '<button class="layout-btn" data-layout="masonry" title="Masonry Layout" aria-label="Switch to masonry gallery layout">',
        '  <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">',
        '    <rect x="3" y="3" width="7" height="7" rx="1"/>',
        '    <rect x="3" y="12" width="7" height="9" rx="1"/>',
        '    <rect x="14" y="3" width="7" height="11" rx="1"/>',
        '    <rect x="14" y="16" width="7" height="5" rx="1"/>',
        "  </svg>",
        "</button>",
        '<button class="layout-btn" data-layout="grid" title="Grid Layout" aria-label="Switch to grid gallery layout">',
        '  <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">',
        '    <rect x="3" y="3" width="7" height="7" rx="1"/>',
        '    <rect x="14" y="3" width="7" height="7" rx="1"/>',
        '    <rect x="3" y="14" width="7" height="7" rx="1"/>',
        '    <rect x="14" y="14" width="7" height="7" rx="1"/>',
        "  </svg>",
        "</button>"
      ].join("");
      switcher.querySelectorAll(".layout-btn").forEach((button) => {
        button.addEventListener("click", (event) => {
          event.stopPropagation();
          const layout = button.getAttribute("data-layout");
          onSwitch(layout);
          switcher.querySelectorAll(".layout-btn").forEach((item) => item.classList.remove("active"));
          button.classList.add("active");
        });
      });
      const activeButton = switcher.querySelector('[data-layout="' + defaultLayout + '"]');
      if (activeButton) {
        activeButton.classList.add("active");
      }
      return switcher;
    }
    processIndividualImages(figures) {
      if (!this.lightbox) {
        return;
      }
      figures.forEach((figure) => {
        const img = figure.querySelector("img");
        const caption = figure.querySelector(".image-caption");
        if (!img) {
          return;
        }
        const galleryId = "single-image-" + String(this.singleImageCount);
        this.singleImageCount += 1;
        const src = figure.getAttribute("data-image-src") || img.currentSrc || img.src;
        const width = parseInt(figure.getAttribute("data-image-width"), 10) || img.naturalWidth || 800;
        const height = parseInt(figure.getAttribute("data-image-height"), 10) || img.naturalHeight || 600;
        this.lightbox.registerGallery(galleryId, [{
          src,
          width,
          height,
          alt: img.alt || "",
          captionHTML: caption ? caption.innerHTML.trim() : ""
        }]);
        figure.classList.add("single-image");
        const trigger = figure.querySelector(".image-container") || img;
        trigger.classList.add("lightbox-trigger");
        trigger.addEventListener("click", () => {
          this.lightbox.open(galleryId, 0, { triggerElement: trigger });
        });
      });
    }
    destroy() {
      this.layoutManager.destroy();
      if (this.lightbox) {
        this.lightbox.destroy();
      }
    }
  };
  var imageGallery = new ImageGallery();
  window.ImageGallery = ImageGallery;
  window.imageGallery = imageGallery;
})();
