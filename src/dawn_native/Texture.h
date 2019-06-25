// Copyright 2017 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DAWNNATIVE_TEXTURE_H_
#define DAWNNATIVE_TEXTURE_H_

#include "dawn_native/Error.h"
#include "dawn_native/Forward.h"
#include "dawn_native/ObjectBase.h"

#include "dawn_native/dawn_platform.h"

#include <vector>

namespace dawn_native {
    MaybeError ValidateTextureDescriptor(DeviceBase* device, const TextureDescriptor* descriptor);
    MaybeError ValidateTextureViewDescriptor(const DeviceBase* device,
                                             const TextureBase* texture,
                                             const TextureViewDescriptor* descriptor);

    bool IsValidSampleCount(uint32_t sampleCount);

    static constexpr dawn::TextureUsageBit kReadOnlyTextureUsages =
        dawn::TextureUsageBit::TransferSrc | dawn::TextureUsageBit::Sampled |
        dawn::TextureUsageBit::Present;

    static constexpr dawn::TextureUsageBit kWritableTextureUsages =
        dawn::TextureUsageBit::TransferDst | dawn::TextureUsageBit::Storage |
        dawn::TextureUsageBit::OutputAttachment;

    struct Format {
        enum Aspect {
            Color,
            Depth,
            Stencil,
            DepthStencil,
        };

        dawn::TextureFormat format;
        bool isRenderable;
        bool isCompressed;
        Aspect aspect;

        uint32_t blockByteSize;
        uint32_t blockWidth;
        uint32_t blockHeight;

        bool IsColor() const;
        bool HasDepth() const;
        bool HasStencil() const;
        bool HasDepthOrStencil() const;
    };

    // Returns the Format corresponding to the dawn::TextureFormat or an error if the format
    // isn't valid.
    ResultOrError<Format> ConvertFormat(dawn::TextureFormat format);

    // Returns the Format corresponding to the dawn::TextureFormat and assumes the format is valid.
    Format ConvertValidFormat(dawn::TextureFormat format);

    class TextureBase : public ObjectBase {
      public:
        enum class TextureState { OwnedInternal, OwnedExternal, Destroyed };

        TextureBase(DeviceBase* device, const TextureDescriptor* descriptor, TextureState state);

        static TextureBase* MakeError(DeviceBase* device);

        dawn::TextureDimension GetDimension() const;
        const Format& GetFormat() const;
        const Extent3D& GetSize() const;
        uint32_t GetArrayLayers() const;
        uint32_t GetNumMipLevels() const;
        uint32_t GetSampleCount() const;
        dawn::TextureUsageBit GetUsage() const;
        TextureState GetTextureState() const;
        uint32_t GetSubresourceIndex(uint32_t mipLevel, uint32_t arraySlice) const;
        bool IsSubresourceContentInitialized(uint32_t baseMipLevel,
                                             uint32_t levelCount,
                                             uint32_t baseArrayLayer,
                                             uint32_t layerCount) const;
        void SetIsSubresourceContentInitialized(uint32_t baseMipLevel,
                                                uint32_t levelCount,
                                                uint32_t baseArrayLayer,
                                                uint32_t layerCount);

        MaybeError ValidateCanUseInSubmitNow() const;

        bool IsMultisampledTexture() const;

        Extent3D GetMipLevelSize(uint64_t level) const;

        // Dawn API
        TextureViewBase* CreateDefaultView();
        TextureViewBase* CreateView(const TextureViewDescriptor* descriptor);
        void Destroy();

      protected:
        void DestroyInternal();

      private:
        TextureBase(DeviceBase* device, ObjectBase::ErrorTag tag);
        virtual void DestroyImpl();

        MaybeError ValidateDestroy() const;
        dawn::TextureDimension mDimension;
        // TODO(cwallez@chromium.org): This should be deduplicated in the Device
        Format mFormat;
        Extent3D mSize;
        uint32_t mArrayLayerCount;
        uint32_t mMipLevelCount;
        uint32_t mSampleCount;
        dawn::TextureUsageBit mUsage = dawn::TextureUsageBit::None;
        TextureState mState;

        // TODO(natlee@microsoft.com): Use a more optimized data structure to save space
        std::vector<bool> mIsSubresourceContentInitializedAtIndex;
    };

    class TextureViewBase : public ObjectBase {
      public:
        TextureViewBase(TextureBase* texture, const TextureViewDescriptor* descriptor);

        static TextureViewBase* MakeError(DeviceBase* device);

        const TextureBase* GetTexture() const;
        TextureBase* GetTexture();

        const Format& GetFormat() const;
        uint32_t GetBaseMipLevel() const;
        uint32_t GetLevelCount() const;
        uint32_t GetBaseArrayLayer() const;
        uint32_t GetLayerCount() const;

      private:
        TextureViewBase(DeviceBase* device, ObjectBase::ErrorTag tag);

        Ref<TextureBase> mTexture;

        // TODO(cwallez@chromium.org): This should be deduplicated in the Device
        Format mFormat;
        uint32_t mBaseMipLevel;
        uint32_t mMipLevelCount;
        uint32_t mBaseArrayLayer;
        uint32_t mArrayLayerCount;
    };

}  // namespace dawn_native

#endif  // DAWNNATIVE_TEXTURE_H_
