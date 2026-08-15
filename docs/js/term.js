(() => {
  // ns-hugo-imp:E:\workspace\notebook\hugo\cpp_notes\themes\hugo-narrow\assets\js\clipboard.js
  async function copyText(text) {
    if (!navigator.clipboard?.writeText || !window.isSecureContext) {
      return false;
    }
    try {
      await navigator.clipboard.writeText(text);
      return true;
    } catch (_) {
      return false;
    }
  }

  // <stdin>
  var COPY_FEEDBACK_MS = 2e3;
  var copyTimers = /* @__PURE__ */ new WeakMap();
  function setCopyFeedback(button, copied) {
    const text = button.querySelector(".copy-text");
    const label = copied ? button.dataset.labelCopied : button.dataset.labelCopy;
    if (text) text.textContent = label || "";
    button.classList.toggle("text-copied", copied);
    button.title = label || "";
    button.setAttribute("aria-label", label || "");
  }
  async function handleCopy(button) {
    const group = button.closest("[data-term-group]");
    if (!group) return;
    const copied = await copyText(group.dataset.copy ?? "");
    if (!copied) return;
    setCopyFeedback(button, true);
    const previousTimer = copyTimers.get(button);
    if (previousTimer) {
      window.clearTimeout(previousTimer);
    }
    const timer = window.setTimeout(() => {
      setCopyFeedback(button, false);
      copyTimers.delete(button);
    }, COPY_FEEDBACK_MS);
    copyTimers.set(button, timer);
  }
  function handleClick(event) {
    const button = event.target.closest("[data-term-copy]");
    if (!button) return;
    event.preventDefault();
    handleCopy(button);
  }
  var initialized = false;
  function initTermBlocks(root = document) {
    if (initialized) return;
    initialized = true;
    document.addEventListener("click", handleClick);
  }
  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", initTermBlocks, { once: true });
  } else {
    initTermBlocks();
  }
})();
