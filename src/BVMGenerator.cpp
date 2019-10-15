#include <ShaderDebugger/BVMGenerator.h>

#include <aGen/aGen.hpp>

// Glslang includes
#include <glslang/MachineIndependent/SymbolTable.h>
#include <glslang/Include/Common.h>
#include <glslang/Include/revision.h>

#include <fstream>
#include <iomanip>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <vector>

namespace sd {
	class TGlslangToBVMTraverser : public glslang::TIntermTraverser {
	public:
		TGlslangToBVMTraverser(const glslang::TIntermediate*);
		virtual ~TGlslangToBVMTraverser() { }

		void init() { m_gen.SetHeader(1, 3); m_isHLSL = false; }

		bool visitAggregate(glslang::TVisit, glslang::TIntermAggregate*);
		bool visitBinary(glslang::TVisit, glslang::TIntermBinary*);
		void visitConstantUnion(glslang::TIntermConstantUnion*);
		bool visitSelection(glslang::TVisit, glslang::TIntermSelection*);
		bool visitSwitch(glslang::TVisit, glslang::TIntermSwitch*);
		void visitSymbol(glslang::TIntermSymbol* symbol);
		bool visitUnary(glslang::TVisit, glslang::TIntermUnary*);
		bool visitLoop(glslang::TVisit, glslang::TIntermLoop*);
		bool visitBranch(glslang::TVisit visit, glslang::TIntermBranch*);

		int getSymbolId(const glslang::TIntermSymbol* symbol);
		int createSpvVariable(const glslang::TIntermSymbol* node);

		void dump(std::vector<uint8_t>& out);

	protected:
		std::vector<ASTVariable> m_vars;

		ag::Generator m_gen;

		bool m_isHLSL;

		TGlslangToBVMTraverser(TGlslangToBVMTraverser&);
		TGlslangToBVMTraverser& operator=(TGlslangToBVMTraverser&);
	};
	
	// translate to flags
	uint32_t TranslateBuiltInDecoration(glslang::TBuiltInVariable builtIn, bool memberDeclaration)
	{
		return (int)builtIn;
		/*
		switch (builtIn) {
		    case glslang::EbvPointSize:			return spv::BuiltInPointSize;
			case glslang::EbvPosition:			return spv::BuiltInPosition;
			case glslang::EbvVertexId:			return spv::BuiltInVertexId;
			case glslang::EbvInstanceId:		return spv::BuiltInInstanceId;
			case glslang::EbvVertexIndex:		return spv::BuiltInVertexIndex;
			case glslang::EbvInstanceIndex:		return spv::BuiltInInstanceIndex;
			case glslang::EbvFragCoord:			return spv::BuiltInFragCoord;
			case glslang::EbvPointCoord:		return spv::BuiltInPointCoord;
			case glslang::EbvFace:				return spv::BuiltInFrontFacing;
			case glslang::EbvFragDepth:			return spv::BuiltInFragDepth;
			case glslang::EbvClipDistance:		return spv::BuiltInClipDistance;
			case glslang::EbvCullDistance:		return spv::BuiltInCullDistance;
			case glslang::EbvViewportIndex:		return spv::BuiltInViewportIndex;
			case glslang::EbvSampleId:			return spv::BuiltInSampleId;
			case glslang::EbvSamplePosition:	return spv::BuiltInSamplePosition;
			case glslang::EbvSampleMask:		return spv::BuiltInSampleMask;
			case glslang::EbvLayer:				return spv::BuiltInLayer;
			case glslang::EbvBaseVertex: 		return spv::BuiltInBaseVertex;
			case glslang::EbvBaseInstance: 		return spv::BuiltInBaseInstance;
			case glslang::EbvDrawId: 			return spv::BuiltInDrawIndex;
			case glslang::EbvPrimitiveId: 		return spv::BuiltInPrimitiveId;
			case glslang::EbvFragStencilRef: 	return spv::BuiltInFragStencilRefEXT;
			case glslang::EbvInvocationId:		return spv::BuiltInInvocationId;
			case glslang::EbvTessLevelInner:	return spv::BuiltInTessLevelInner;
			case glslang::EbvTessLevelOuter:	return spv::BuiltInTessLevelOuter;
			case glslang::EbvTessCoord:			return spv::BuiltInTessCoord;
			case glslang::EbvPatchVertices:		return spv::BuiltInPatchVertices;
			case glslang::EbvHelperInvocation:	return spv::BuiltInHelperInvocation;
			case glslang::EbvNumWorkGroups:		return spv::BuiltInNumWorkgroups;
			case glslang::EbvWorkGroupSize:		return spv::BuiltInWorkgroupSize;
			case glslang::EbvWorkGroupId:		return spv::BuiltInWorkgroupId;
			case glslang::EbvLocalInvocationId:	return spv::BuiltInLocalInvocationId;
			case glslang::EbvLocalInvocationIndex:return spv::BuiltInLocalInvocationIndex;
			case glslang::EbvGlobalInvocationId:return spv::BuiltInGlobalInvocationId;
			case glslang::EbvSubGroupSize: 		return spv::BuiltInSubgroupSize;
			case glslang::EbvSubGroupInvocation:return spv::BuiltInSubgroupLocalInvocationId;
			case glslang::EbvNumSubgroups:		return spv::BuiltInNumSubgroups;
			case glslang::EbvSubgroupID:		return spv::BuiltInSubgroupId;

		default:
			return spv::BuiltInMax;
		}
		*/
	}

	std::string convertGlslangToSpvType(const glslang::TType& type,
		glslang::TLayoutPacking explicitLayout, const glslang::TQualifier& qualifier,
		bool lastBufferBlockMember, bool forwardReferenceOnly)
	{
		switch (type.getBasicType()) {
		case glslang::EbtVoid: return "void";
		case glslang::EbtBool: return "bool";
		case glslang::EbtInt: return "int";
		case glslang::EbtUint: return "uint";
		case glslang::EbtFloat: return "float";
		case glslang::EbtDouble: return "double";
		case glslang::EbtFloat16: return "float16"
		case glslang::EbtInt8: return "int8";
		case glslang::EbtUint8: return "uint8";
		case glslang::EbtInt16: return "int16";
		case glslang::EbtUint16: return "uint16";
		case glslang::EbtInt64: return "int64";
		case glslang::EbtUint64: return "uint64";
		case glslang::EbtAtomicUint: return "atomic_uint";
		case glslang::EbtSampler:
			{
				return "sampler";
				/*
				TODO: samplers
				const glslang::TSampler& sampler = type.getSampler();
				if (sampler.isPureSampler()) {
					spvType = builder.makeSamplerType();
				} else {
					// an image is present, make its type
					spvType = builder.makeImageType(getSampledType(sampler), TranslateDimensionality(sampler),
													sampler.isShadow(), sampler.isArrayed(), sampler.isMultiSample(),
													sampler.isImageClass() ? 2 : 1, TranslateImageFormat(type));
					if (sampler.isCombined()) {
						// already has both image and sampler, make the combined type
						spvType = builder.makeSampledImageType(spvType);
					}
				}
				*/
			}
			break;
		case glslang::EbtStruct:
		case glslang::EbtBlock:
			{
				return "struct";
				/*
				TODO: structures
				// If we've seen this struct type, return it
				const glslang::TTypeList* glslangMembers = type.getStruct();

				// Try to share structs for different layouts, but not yet for other
				// kinds of qualification (primarily not yet including interpolant qualification).
				if (! HasNonLayoutQualifiers(type, qualifier))
					spvType = structMap[explicitLayout][qualifier.layoutMatrix][glslangMembers];
				if (spvType != spv::NoResult)
					break;

				// else, we haven't seen it...
				if (type.getBasicType() == glslang::EbtBlock)
					memberRemapper[glslangMembers].resize(glslangMembers->size());
				spvType = convertGlslangStructToSpvType(type, glslangMembers, explicitLayout, qualifier);
				*/
			}
			break;
		default:
			assert(0);
			break;
		}

		if (type.isMatrix())
			spvType = builder.makeMatrixType(spvType, type.getMatrixCols(), type.getMatrixRows());
		else {
			// If this variable has a vector element count greater than 1, create a SPIR-V vector
			if (type.getVectorSize() > 1)
				spvType = builder.makeVectorType(spvType, type.getVectorSize());
		}

		if (type.isCoopMat()) {
			builder.addCapability(spv::CapabilityCooperativeMatrixNV);
			builder.addExtension(spv::E_SPV_NV_cooperative_matrix);
			if (type.getBasicType() == glslang::EbtFloat16)
				builder.addCapability(spv::CapabilityFloat16);
			if (type.getBasicType() == glslang::EbtUint8 ||
				type.getBasicType() == glslang::EbtInt8) {
				builder.addCapability(spv::CapabilityInt8);
			}

			spv::Id scope = makeArraySizeId(*type.getTypeParameters(), 1);
			spv::Id rows = makeArraySizeId(*type.getTypeParameters(), 2);
			spv::Id cols = makeArraySizeId(*type.getTypeParameters(), 3);

			spvType = builder.makeCooperativeMatrixType(spvType, scope, rows, cols);
		}

		if (type.isArray()) {
			int stride = 0;  // keep this 0 unless doing an explicit layout; 0 will mean no decoration, no stride

			// Do all but the outer dimension
			if (type.getArraySizes()->getNumDims() > 1) {
				// We need to decorate array strides for types needing explicit layout, except blocks.
				if (explicitLayout != glslang::ElpNone && type.getBasicType() != glslang::EbtBlock) {
					// Use a dummy glslang type for querying internal strides of
					// arrays of arrays, but using just a one-dimensional array.
					glslang::TType simpleArrayType(type, 0); // deference type of the array
					while (simpleArrayType.getArraySizes()->getNumDims() > 1)
						simpleArrayType.getArraySizes()->dereference();

					// Will compute the higher-order strides here, rather than making a whole
					// pile of types and doing repetitive recursion on their contents.
					stride = getArrayStride(simpleArrayType, explicitLayout, qualifier.layoutMatrix);
				}

				// make the arrays
				for (int dim = type.getArraySizes()->getNumDims() - 1; dim > 0; --dim) {
					spvType = builder.makeArrayType(spvType, makeArraySizeId(*type.getArraySizes(), dim), stride);
					if (stride > 0)
						builder.addDecoration(spvType, spv::DecorationArrayStride, stride);
					stride *= type.getArraySizes()->getDimSize(dim);
				}
			} else {
				// single-dimensional array, and don't yet have stride

				// We need to decorate array strides for types needing explicit layout, except blocks.
				if (explicitLayout != glslang::ElpNone && type.getBasicType() != glslang::EbtBlock)
					stride = getArrayStride(type, explicitLayout, qualifier.layoutMatrix);
			}

			// Do the outer dimension, which might not be known for a runtime-sized array.
			// (Unsized arrays that survive through linking will be runtime-sized arrays)
			if (type.isSizedArray())
				spvType = builder.makeArrayType(spvType, makeArraySizeId(*type.getArraySizes(), 0), stride);
			else {
				if (!lastBufferBlockMember) {
					builder.addIncorporatedExtension("SPV_EXT_descriptor_indexing", spv::Spv_1_5);
					builder.addCapability(spv::CapabilityRuntimeDescriptorArrayEXT);
				}
				spvType = builder.makeRuntimeArray(spvType);
			}
			if (stride > 0)
				builder.addDecoration(spvType, spv::DecorationArrayStride, stride);
		}

		return spvType;
	}

	int TGlslangToBVMTraverser::createSpvVariable(const glslang::TIntermSymbol* node)
	{
		if (node->getQualifier().isConstant()) {
			/*
			spv::Id result = createSpvConstant(*node);
			if (result != spv::NoResult)
				return result;
				*/
			return -1; // TODO: createSpvConstant
		}

		// Now, handle actual variables
		ASTVariable var;
		var.ID = node->getId();
		
		const auto& type = node->getType();

		if (type.getQualifier().isPipeInput())
			var.IsInput = true;
		if (type.getQualifier().isPipeOutput())
			var.IsOutput = true;

		if (type.getQualifier().storage == glslang::EvqUniform) {
			var.IsUniform = true;

			// TODO: constant uniforms
			//if (type.isAtomic())
			//	return spv::StorageClassAtomicCounter;
			//if (type.containsOpaque())
			//	return spv::StorageClassUniformConstant;
		}
		if (type.getQualifier().storage == glslang::EvqUniform)
			var.IsBuffer = true;

		/*
		TODO:
		if (type.getQualifier().isUniformOrBuffer()) {
			if (type.getQualifier().isPushConstant())
				return spv::StorageClassPushConstant;
			if (type.getBasicType() == glslang::EbtBlock)
				return spv::StorageClassUniform;
			return spv::StorageClassUniformConstant;
		}
		*/
	
		switch (type.getQualifier().storage) {
			case glslang::EvqGlobal: var.IsGlobal = true; break;
			case glslang::EvqShared: var.IsShared = true; break;
		}
		
		var.Type = convertGlslangToSpvType(node->getType());

		var.Name = std::string(node->getName().c_str());
		if (glslang::IsAnonymous(var.Name.c_str()))
			var.Name = ""; // TODO: =

		// TODO: gen
		

		return builder.createVariable(storageClass, spvType, name);
	}

	int TGlslangToBVMTraverser::getSymbolId(const glslang::TIntermSymbol* symbol)
	{
		for (int i = 0; i < m_vars.size(); i++)
			if (m_vars[i].ID == symbol->getId())
				return m_vars[i].ID;

		// it was not found, create it
		uint32_t builtIn = TranslateBuiltInDecoration(symbol->getQualifier().builtIn, false);
		id = createSpvVariable(symbol, forcedType.first);
		symbolValues[symbol->getId()] = id;
		if (forcedType.second != spv::NoType)
			forceType[id] = forcedType.second;

		if (symbol->getBasicType() != glslang::EbtBlock) {
			builder.addDecoration(id, TranslatePrecisionDecoration(symbol->getType()));
			builder.addDecoration(id, TranslateInterpolationDecoration(symbol->getType().getQualifier()));
			builder.addDecoration(id, TranslateAuxiliaryStorageDecoration(symbol->getType().getQualifier()));
	
			addMeshNVDecoration(id, /*member*/ -1, symbol->getType().getQualifier());
			if (symbol->getQualifier().hasComponent())
				builder.addDecoration(id, spv::DecorationComponent, symbol->getQualifier().layoutComponent);
			if (symbol->getQualifier().hasIndex())
				builder.addDecoration(id, spv::DecorationIndex, symbol->getQualifier().layoutIndex);
	
			if (symbol->getType().getQualifier().hasSpecConstantId())
				builder.addDecoration(id, spv::DecorationSpecId, symbol->getType().getQualifier().layoutSpecConstantId);
			// atomic counters use this:
			if (symbol->getQualifier().hasOffset())
				builder.addDecoration(id, spv::DecorationOffset, symbol->getQualifier().layoutOffset);
		}

		if (symbol->getQualifier().hasLocation())
			builder.addDecoration(id, spv::DecorationLocation, symbol->getQualifier().layoutLocation);
		builder.addDecoration(id, TranslateInvariantDecoration(symbol->getType().getQualifier()));
		if (symbol->getQualifier().hasStream() && glslangIntermediate->isMultiStream()) {
			builder.addCapability(spv::CapabilityGeometryStreams);
			builder.addDecoration(id, spv::DecorationStream, symbol->getQualifier().layoutStream);
		}
		if (symbol->getQualifier().hasSet())
			builder.addDecoration(id, spv::DecorationDescriptorSet, symbol->getQualifier().layoutSet);
		else if (IsDescriptorResource(symbol->getType())) {
			// default to 0
			builder.addDecoration(id, spv::DecorationDescriptorSet, 0);
		}
		if (symbol->getQualifier().hasBinding())
			builder.addDecoration(id, spv::DecorationBinding, symbol->getQualifier().layoutBinding);
		else if (IsDescriptorResource(symbol->getType())) {
			// default to 0
			builder.addDecoration(id, spv::DecorationBinding, 0);
		}
		if (symbol->getQualifier().hasAttachment())
			builder.addDecoration(id, spv::DecorationInputAttachmentIndex, symbol->getQualifier().layoutAttachment);
		if (glslangIntermediate->getXfbMode()) {
			builder.addCapability(spv::CapabilityTransformFeedback);
			if (symbol->getQualifier().hasXfbBuffer()) {
				builder.addDecoration(id, spv::DecorationXfbBuffer, symbol->getQualifier().layoutXfbBuffer);
				unsigned stride = glslangIntermediate->getXfbStride(symbol->getQualifier().layoutXfbBuffer);
				if (stride != glslang::TQualifier::layoutXfbStrideEnd)
					builder.addDecoration(id, spv::DecorationXfbStride, stride);
			}
			if (symbol->getQualifier().hasXfbOffset())
				builder.addDecoration(id, spv::DecorationOffset, symbol->getQualifier().layoutXfbOffset);
		}

		// add built-in variable decoration
		if (builtIn != spv::BuiltInMax) {
			builder.addDecoration(id, spv::DecorationBuiltIn, (int)builtIn);
		}

		if (symbol->getType().isImage()) {
			std::vector<spv::Decoration> memory;
			TranslateMemoryDecoration(symbol->getType().getQualifier(), memory, glslangIntermediate->usingVulkanMemoryModel());
			for (unsigned int i = 0; i < memory.size(); ++i)
				builder.addDecoration(id, memory[i]);
		}

		// nonuniform
		builder.addDecoration(id, TranslateNonUniformDecoration(symbol->getType().getQualifier()));

		if (builtIn == spv::BuiltInSampleMask) {
			spv::Decoration decoration;
			// GL_NV_sample_mask_override_coverage extension
			if (glslangIntermediate->getLayoutOverrideCoverage())
				decoration = (spv::Decoration)spv::DecorationOverrideCoverageNV;
			else
				decoration = (spv::Decoration)spv::DecorationMax;
			builder.addDecoration(id, decoration);
			if (decoration != spv::DecorationMax) {
				builder.addCapability(spv::CapabilitySampleMaskOverrideCoverageNV);
				builder.addExtension(spv::E_SPV_NV_sample_mask_override_coverage);
			}
		}
		else if (builtIn == spv::BuiltInLayer) {
			// SPV_NV_viewport_array2 extension
			if (symbol->getQualifier().layoutViewportRelative) {
				builder.addDecoration(id, (spv::Decoration)spv::DecorationViewportRelativeNV);
				builder.addCapability(spv::CapabilityShaderViewportMaskNV);
				builder.addExtension(spv::E_SPV_NV_viewport_array2);
			}
			if (symbol->getQualifier().layoutSecondaryViewportRelativeOffset != -2048) {
				builder.addDecoration(id, (spv::Decoration)spv::DecorationSecondaryViewportRelativeNV,
									symbol->getQualifier().layoutSecondaryViewportRelativeOffset);
				builder.addCapability(spv::CapabilityShaderStereoViewNV);
				builder.addExtension(spv::E_SPV_NV_stereo_view_rendering);
			}
		}

		if (symbol->getQualifier().layoutPassthrough) {
			builder.addDecoration(id, spv::DecorationPassthroughNV);
			builder.addCapability(spv::CapabilityGeometryShaderPassthroughNV);
			builder.addExtension(spv::E_SPV_NV_geometry_shader_passthrough);
		}
		if (symbol->getQualifier().pervertexNV) {
			builder.addDecoration(id, spv::DecorationPerVertexNV);
			builder.addCapability(spv::CapabilityFragmentBarycentricNV);
			builder.addExtension(spv::E_SPV_NV_fragment_shader_barycentric);
		}

		if (glslangIntermediate->getHlslFunctionality1() && symbol->getType().getQualifier().semanticName != nullptr) {
			builder.addExtension("SPV_GOOGLE_hlsl_functionality1");
			builder.addDecoration(id, (spv::Decoration)spv::DecorationHlslSemanticGOOGLE,
								symbol->getType().getQualifier().semanticName);
		}

		if (symbol->isReference()) {
			builder.addDecoration(id, symbol->getType().getQualifier().restrict ? spv::DecorationRestrictPointerEXT : spv::DecorationAliasedPointerEXT);
		}

		return id;
	}

	void TGlslangToBVMTraverser::visitSymbol(glslang::TIntermSymbol* symbol)
	{
		// set up all the IO decorations on the first call
		int id = getSymbolId(symbol);

		if (builder.isPointer(id)) {
			// Include all "static use" and "linkage only" interface variables on the OpEntryPoint instruction
			// Consider adding to the OpEntryPoint interface list.
			// Only looking at structures if they have at least one member.
			if (!symbol->getType().isStruct() || symbol->getType().getStruct()->size() > 0) {
				spv::StorageClass sc = builder.getStorageClass(id);
				// Before SPIR-V 1.4, we only want to include Input and Output.
				// Starting with SPIR-V 1.4, we want all globals.
				if ((glslangIntermediate->getSpv().spv >= glslang::EShTargetSpv_1_4 && sc != spv::StorageClassFunction) ||
					(sc == spv::StorageClassInput || sc == spv::StorageClassOutput)) {
					iOSet.insert(id);
				}
			}

			// If the SPIR-V type is required to be different than the AST type,
			// translate now from the SPIR-V type to the AST type, for the consuming
			// operation.
			// Note this turns it from an l-value to an r-value.
			// Currently, all symbols needing this are inputs; avoid the map lookup when non-input.
			if (symbol->getType().getQualifier().storage == glslang::EvqVaryingIn)
				id = translateForcedType(id);
		}

		// Only process non-linkage-only nodes for generating actual static uses
		if (! linkageOnly || symbol->getQualifier().isSpecConstant()) {
			// Prepare to generate code for the access

			// L-value chains will be computed left to right.  We're on the symbol now,
			// which is the left-most part of the access chain, so now is "clear" time,
			// followed by setting the base.
			builder.clearAccessChain();

			// For now, we consider all user variables as being in memory, so they are pointers,
			// except for
			// A) R-Value arguments to a function, which are an intermediate object.
			//    See comments in handleUserFunctionCall().
			// B) Specialization constants (normal constants don't even come in as a variable),
			//    These are also pure R-values.
			// C) R-Values from type translation, see above call to translateForcedType()
			glslang::TQualifier qualifier = symbol->getQualifier();
			if (qualifier.isSpecConstant() || rValueParameters.find(symbol->getId()) != rValueParameters.end() ||
				!builder.isPointerType(builder.getTypeId(id)))
				builder.setAccessChainRValue(id);
			else
				builder.setAccessChainLValue(id);
		}

		// Process linkage-only nodes for any special additional interface work.
		if (linkageOnly) {
			if (glslangIntermediate->getHlslFunctionality1()) {
				// Map implicit counter buffers to their originating buffers, which should have been
				// seen by now, given earlier pruning of unused counters, and preservation of order
				// of declaration.
				if (symbol->getType().getQualifier().isUniformOrBuffer()) {
					if (!glslangIntermediate->hasCounterBufferName(symbol->getName())) {
						// Save possible originating buffers for counter buffers, keyed by
						// making the potential counter-buffer name.
						std::string keyName = symbol->getName().c_str();
						keyName = glslangIntermediate->addCounterBufferName(keyName);
						counterOriginator[keyName] = symbol;
					} else {
						// Handle a counter buffer, by finding the saved originating buffer.
						std::string keyName = symbol->getName().c_str();
						auto it = counterOriginator.find(keyName);
						if (it != counterOriginator.end()) {
							id = getSymbolId(it->second);
							if (id != spv::NoResult) {
								spv::Id counterId = getSymbolId(symbol);
								if (counterId != spv::NoResult) {
									builder.addExtension("SPV_GOOGLE_hlsl_functionality1");
									builder.addDecorationId(id, spv::DecorationHlslCounterBufferGOOGLE, counterId);
								}
							}
						}
					}
				}
			}
		}
	}

	void BVMGenerator::Translate(const glslang::TIntermediate& intermediate, std::vector<uint8_t>& out)
	{
		TIntermNode* root = intermediate.getTreeRoot();

		if (root == 0)
			return;

		glslang::GetThreadPoolAllocator().push();

		TGlslangToBVMTraverser it(&intermediate);
		root->traverse(&it);
		it.dump(out);

		glslang::GetThreadPoolAllocator().pop();
	}
}