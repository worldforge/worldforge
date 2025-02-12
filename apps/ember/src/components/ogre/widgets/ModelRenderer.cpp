//
// C++ Implementation: ModelRenderer
//
// Description:
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2005
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#include "ModelRenderer.h"

#include "EntityCEGUITexture.h"
#include "components/ogre/model/Model.h"
#include "components/ogre/model/ModelMount.h"
#include "components/ogre/model/ModelDefinitionManager.h"

#include <OgreSceneNode.h>

#include <components/ogre/SceneNodeProvider.h>


namespace Ember::OgreView::Gui {

ModelRenderer::ModelRenderer(CEGUI::Window* image, const std::string& name) :
		MovableObjectRenderer(image, name),
		mModel(nullptr),
		mModelMount(nullptr),
		mDefaultTranslation(Ogre::Vector3::ZERO),
		mDefaultRotation(Ogre::Quaternion::IDENTITY) {
}

ModelRenderer::~ModelRenderer() = default;

void ModelRenderer::showModel() {
	if (mModel) {
		repositionSceneNode();
		rescaleAxisMarker();
		mTexture->getRenderContext()->repositionCamera();
		if (mAutoShowFull) {
			mModelReloadedConnection = mModel->Reloaded.connect(sigc::mem_fun(*this, &ModelRenderer::model_Reloaded));
			showFull();
		}
	}

}

void ModelRenderer::repositionSceneNode() {
	if (mModel) {
		Ogre::SceneNode* node = mTexture->getRenderContext()->getSceneNode();
		if (node) {
			node->setOrientation(mDefaultRotation);
			node->rotate(mModel->getDefinition()->getRotation());

			//translate the scale node according to the translate defined in the model
			node->setPosition(mDefaultTranslation);
			node->translate(mModel->getDefinition()->getTranslate());

		}
	}
}

void ModelRenderer::updateRender() {
	if (mModel && !mModel->isLoaded()) {
		//If it's being loaded in a background thread, listen for reloading and render it then. The "Reload" signal will be emitted in the main thread.
		mModelDelayedUpdateConnection.disconnect();
		mModelDelayedUpdateConnection = mModel->Reloaded.connect(sigc::mem_fun(*this, &ModelRenderer::delayedUpdateRender));
	} else {
		MovableObjectRenderer::updateRender();
	}
}

void ModelRenderer::delayedUpdateRender() {
	if (mAutoShowFull) {
		showFull();
	}
	repositionSceneNode();
	mTexture->getRenderContext()->repositionCamera();
	updateRender();
	mModelDelayedUpdateConnection.disconnect();
}

Model::Model* ModelRenderer::getModel() {
	if (mModel) {
		return mModel.get();
	}
	return nullptr;
}

void ModelRenderer::showModel(const std::string& modelName, const Ogre::Vector3& translation, const Ogre::Quaternion& orientation) {
	auto modelDef = Model::ModelDefinitionManager::getSingleton().getByName(modelName);
	showModel(modelDef, translation, orientation);

}

void ModelRenderer::showModel(const Model::ModelDefinitionPtr& modelDef, const Ogre::Vector3& translation, const Ogre::Quaternion& orientation) {
	mDefaultRotation = orientation;
	mDefaultTranslation = translation;
	if (mModel) {
		mModelMount.reset();
		mModel.reset();
		mModelReloadedConnection.disconnect();
		mModelDelayedUpdateConnection.disconnect();
		//delete mModel;
	}
	if (modelDef) {
		mModel = std::make_unique<Model::Model>(*mTexture->getRenderContext()->getSceneManager(), modelDef, "");
		//override the rendering distance from the model; we want to always show it in the preview
		mModel->setRenderingDistance(0);

		mModelMount = std::make_unique<Model::ModelMount>(*mModel, std::make_unique<SceneNodeProvider>(mTexture->getRenderContext()->getSceneNode(), nullptr, false));

		mModel->load();
		showModel();

		mTexture->getRenderContext()->setActive(true);
	} else {
		mTexture->getRenderContext()->setActive(false);
	}
}

void ModelRenderer::model_Reloaded() {
	rescaleAxisMarker();
	showFull();
}

float ModelRenderer::getMovableBoundingRadius() {
	if (mModel) {
		return mModel->getCombinedBoundingRadius();
	}
	return 0;
}


}


